#include "mapping_handler.hpp"

#include <boost/algorithm/string.hpp>
#include <inja/inja.hpp>
#include <unordered_map>
#include <optional>
#include <fstream>
#include <vector>
#include <filesystem>
#include <logging.h>

#include "map_types/custom_mapping.hpp"
#include "map_types/dim_mapping.hpp"
#include "map_types/expr_mapping.hpp"
#include "map_types/plugin_mapping.hpp"
#include "map_types/value_mapping.hpp"

void MappingHandler::reset()
{
    _machine_register.clear();
    _mapping_config.clear();
    _init = false;
}

void MappingHandler::init(bool enable_caching, std::optional<size_t> cache_size)
{
    if (_init || !_machine_register.empty()) {
        return;
    }

    if (enable_caching) {
        auto size = cache_size ? cache_size.value() : ram_cache::default_size;
        _ram_cache = std::make_shared<ram_cache::RamCache>(size);
    } else {
        _ram_cache = nullptr;
    }
    _cache_enabled = _ram_cache != nullptr;

    _init = true;
}

std::optional<MappingPair> MappingHandler::read_mappings(const MachineName& machine, const std::string& request_ids, const std::optional<int> shot)
{
    load_machine(machine);
    if (_machine_register.count(machine) == 0) {
        return {};
    }

    auto& [mappings, attributes] = _machine_register[machine];
    if (mappings.count(request_ids) == 0 || attributes.count(request_ids) == 0) {
        return {};
    }

    int attr_shot = -1;
    int map_shot = -1;

    if (shot) {
        attr_shot = select_shot(attributes[request_ids], shot.value());
        map_shot = select_shot(attributes[request_ids], shot.value());
    }

    if (attributes[request_ids].map.count(attr_shot) == 0 || mappings[request_ids].map.count(attr_shot) == 0) {
        return {};
    }

    nlohmann::json& attr = attributes[request_ids].map[attr_shot];
    IDSMapRegister& map = mappings[request_ids].map[map_shot];

    // AJP :: Safety check if ids request not in mapping json (and typo obviously)
    return {std::make_pair(std::ref(attr), std::ref(map))};
}

void MappingHandler::set_map_dir(const std::string_view mapping_dir)
{
    _mapping_dir = mapping_dir;
}

std::vector<int> MappingHandler::find_mapping_dirs(const MachineName& machine, const IDSName& ids_name) const {
    auto path = _mapping_dir / machine / ids_name;
    std::vector<int> mapping_dirs;

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_directory()) {
            mapping_dirs.push_back(std::stoi(entry.path().filename().string()));
        }
    }

    return mapping_dirs;
}

std::filesystem::path MappingHandler::mapping_path(const MachineName& machine, const IDSName& ids_name, const int shot,
                                                   const std::string& file_name) const {
    if (ids_name.empty()) {
        return _mapping_dir / machine / file_name;
    }

    if (shot < 0) {
        return _mapping_dir / machine / ids_name / file_name;
    }

    return _mapping_dir / machine / ids_name / std::to_string(shot) / file_name;
}

int MappingHandler::load_machine(const MachineName& machine)
{
    if (_machine_register.count(machine) == 1) {
        // machine already loaded
        return 0;
    }

    const auto file_path = mapping_path(machine, "", 0, "mappings.cfg.json");

    std::ifstream map_cfg_file(file_path);
    if (map_cfg_file) {
        map_cfg_file >> _mapping_config;
    } else {
        throw std::runtime_error("MappingHandler::load_configs - Cannot open JSON mapping config file");
    }

    _machine_register[machine] = {{}, {}};

    for (const auto& ids_name : _mapping_config[_dd_version].get<std::vector<std::string>>()) {
        load_globals(machine, ids_name);
        load_mappings(machine, ids_name);
    }

    return 0;
}

nlohmann::json MappingHandler::load_toplevel(const MachineName& machine) const {
    auto file_path = mapping_path(machine, "", 0, "globals.json");

    nlohmann::json toplevel_globals;

    std::ifstream globals_file;
    globals_file.open(file_path);
    if (globals_file) {
        try {
            globals_file >> toplevel_globals;
        } catch (nlohmann::json::exception& ex) {
            std::string json_error{"MappingHandler::load_globals - "};
            json_error.append(ex.what());
            throw std::runtime_error(json_error.c_str());
        }

    } else {
        throw std::runtime_error("MappingHandler::load_globals- Cannot open top-level globals file");
    }
    return toplevel_globals;
}

int MappingHandler::load_shot_globals(const MachineName& machine, const IDSName& ids_name, int shot) {
    auto file_path = mapping_path(machine, ids_name, shot, "globals.json");

    std::ifstream globals_file;
    globals_file.open(file_path);
    if (globals_file) {
        nlohmann::json temp_globals;
        try {
            globals_file >> temp_globals;
        } catch (nlohmann::json::exception& ex) {
            std::string json_error{"MappingHandler::load_globals - "};
            json_error.append(ex.what());
            throw std::runtime_error(json_error.c_str());
        }

        temp_globals.update(load_toplevel(machine));
        _machine_register[machine].attributes[ids_name].map[shot] = temp_globals; // Record globals

    } else {
        throw std::runtime_error("MappingHandler::load_globals - Cannot open JSON globals file");
    }

    return 0;
}

int MappingHandler::load_globals(const MachineName& machine, const IDSName& ids_name)
{
    const auto mapping_dirs = find_mapping_dirs(machine, ids_name);
    if (mapping_dirs.empty()) {
        return load_shot_globals(machine, ids_name, -1);
    }
    for (const int shot : mapping_dirs) {
        int rc = load_shot_globals(machine, ids_name, shot);
        if (rc != 0) { return rc; }
    }
    return 0;
}

int MappingHandler::load_shot_mappings(const MachineName& machine, const IDSName& ids_name, int shot)
{
    auto file_path = mapping_path(machine, ids_name, shot, "mappings.json");

    std::ifstream map_file;
    map_file.open(file_path);
    if (map_file) {
        nlohmann::json temp_mappings;
        try {
            map_file >> temp_mappings;
        } catch (nlohmann::json::exception& ex) {
            std::string json_error{"MappingHandler::load_mappings - "};
            json_error.append(ex.what());
            throw std::runtime_error(json_error.c_str());
        }

        init_mappings(machine, ids_name, temp_mappings, shot);
    } else {
        throw std::runtime_error("MappingHandler::load_mappings - Cannot open JSON mapping file");
    }

    return 0;
}


int MappingHandler::load_mappings(const MachineName& machine, const IDSName& ids_name)
{
    const auto mapping_dirs = find_mapping_dirs(machine, ids_name);
    if (mapping_dirs.empty()) {
        return load_shot_mappings(machine, ids_name, -1);
    }
    for (const int shot : mapping_dirs) {
        int rc = load_shot_mappings(machine, ids_name, shot);
        if (rc != 0) { return rc; }
    }
    return 0;
}

int MappingHandler::init_value_mapping(IDSMapRegister& map_reg, const std::string& key, const nlohmann::json& value)
{
    const auto& value_json = value.at("VALUE");
    map_reg.try_emplace(key, std::make_unique<ValueMapping>(value_json));
    return 0;
}

namespace
{

void apply_config(std::unordered_map<std::string, nlohmann::json>& args, std::optional<std::string>& function,
                  nlohmann::json plugin_config_map, const std::string& plugin_name)
{
    if (plugin_config_map.contains(plugin_name)) {
        const auto& plugin_config = plugin_config_map[plugin_name].get<nlohmann::json>();
        const auto& plugin_args = plugin_config["ARGS"].get<nlohmann::json>();
        for (const auto& [name, arg] : plugin_args.items()) {
            if (args.count(name) == 0) {
                // don't overwrite mapping arguments with global values
                args[name] = arg;
            }
        }
        if (plugin_config.contains("FUNCTION") && !function) {
            function = plugin_config["FUNCTION"].get<std::string>();
        }
    }
}

std::optional<float> get_float_value(const std::string& name, const nlohmann::json& value,
                                     const nlohmann::json& ids_attributes)
{
    std::optional<float> opt_float{std::nullopt};
    if (value.contains(name) and !value[name].is_null()) {
        if (value[name].is_number()) {
            opt_float = value[name].get<float>();
        } else if (value[name].is_string()) {
            try {
                const auto post_inja_str = inja::render(value[name].get<std::string>(), ids_attributes);
                opt_float = std::stof(post_inja_str);
            } catch (const std::invalid_argument&) {
                const std::string message = "\nCannot convert " + name + " string to float\n";
                log(LogLevel::DEBUG, message);
            }
        }
    }
    return opt_float;
}

} // namespace

int MappingHandler::init_plugin_mapping(IDSMapRegister& map_reg, const std::string& key, const nlohmann::json& value,
                                        const nlohmann::json& ids_attributes,
                                        std::shared_ptr<ram_cache::RamCache>& ram_cache)
{
    auto plugin_name = value["PLUGIN"].get<std::string>();
    boost::to_upper(plugin_name);

    auto args = value["ARGS"].get<MapArgs_t>();
    auto offset = get_float_value("OFFSET", value, ids_attributes);
    auto scale = get_float_value("SCALE", value, ids_attributes);
    auto slice = value.contains("SLICE") ? std::optional<std::string>{value["SLICE"].get<std::string>()}
                                         : std::optional<std::string>{};
    auto function = value.contains("FUNCTION") ? std::optional<std::string>{value["FUNCTION"].get<std::string>()}
                                               : std::optional<std::string>{};

    if (ids_attributes.contains("PLUGIN_CONFIG")) {
        const auto& plugin_config_map = ids_attributes["PLUGIN_CONFIG"].get<nlohmann::json>();
        apply_config(args, function, plugin_config_map, plugin_name);
    }

    map_reg.try_emplace(key,
                        std::make_unique<PluginMapping>(plugin_name, args, offset, scale, slice, function, ram_cache));
    return 0;
}

int MappingHandler::init_di_mapping(IDSMapRegister& map_reg, const std::string& key, const nlohmann::json& value)
{
    map_reg.try_emplace(key, std::make_unique<DimMapping>(value["DI_PROBE"].get<std::string>()));
    return 0;
}

int MappingHandler::init_expr_mapping(IDSMapRegister& map_reg, const std::string& key, const nlohmann::json& value)
{
    map_reg.try_emplace(
        key, std::make_unique<ExprMapping>(value["EXPR"].get<std::string>(),
                                           value["PARAMETERS"].get<std::unordered_map<std::string, std::string>>()));
    return 0;
}

int MappingHandler::init_custo_mapping(IDSMapRegister& map_reg, const std::string& key, const nlohmann::json& value)
{
    map_reg.try_emplace(key, std::make_unique<CustomMapping>(value["CUSTO_TYPE"].get<CustomMapType_t>()));
    return 0;
}

int MappingHandler::init_mappings(const MachineName& machine, const IDSName& ids_name, const nlohmann::json& data, int shot)
{
    const auto& attributes = _machine_register[machine].attributes;
    IDSMapRegister temp_map_reg;
    for (const auto& [key, value] : data.items()) {

        switch (value["MAP_TYPE"].get<MappingType>()) {
            case MappingType::VALUE:
                init_value_mapping(temp_map_reg, key, value);
                break;
            case MappingType::PLUGIN:
                init_plugin_mapping(temp_map_reg, key, value, attributes.at(ids_name).map.at(shot), _ram_cache);
                break;
            case MappingType::DIM:
                init_di_mapping(temp_map_reg, key, value);
                break;
            case MappingType::EXPR:
                init_expr_mapping(temp_map_reg, key, value);
                break;
            case MappingType::CUSTOM:
                init_custo_mapping(temp_map_reg, key, value);
                break;
            default:
                break;
        }
    }

    _machine_register[machine].mappings[ids_name].map[shot] = std::move(temp_map_reg);
    log(LogLevel::DEBUG, "calling read function \n");

    return 0;
}
