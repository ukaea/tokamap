#include "library.h"

#include <deque>
#include <regex>
#include <nlohmann/json.hpp>
#include <boost/algorithm/string.hpp>

#include "logging.hpp"
#include "handlers/mapping_handler.hpp"

std::string find_mapping(const IDSMapRegister& mappings, const std::string& path, const std::vector<int>& indices,
                         const std::string& full_path)
{
    // If mapping is found we are good
    if (mappings.count(path) > 0) {
        return path;
    }

    // Check with the path without generalisation
    if (mappings.count(full_path) > 0) {
        return full_path;
    }

    // If there's nothing to replace then no mapping can be found
    if (indices.empty()) {
        return "";
    }

    // Check for last # replaced with index
    std::string new_path = boost::replace_last_copy(path, "#", std::to_string(indices.back()));
    if (mappings.count(new_path) > 0) {
        return new_path;
    }

    // No mappings found
    return "";
}

/**
 * @class TokaMapImpl
 * @brief UDA plugin to map Tokamak data
 *
 * UDA plugin to allow the mapping of experimental fusion data to IMAS data
 * format for a given data dictionary version. Mappings are available in
 * JSON format, which are then parsed and used to
 * (1) read data,
 * (2) apply transformations, and
 * (3) return the data in a format the IDS is expecting
 *
 */
class TokaMapImpl
{
public:
    explicit TokaMapImpl(std::unordered_map<std::string, std::string> options)
        : _options(std::move(options))
    {}

    void init(std::string_view path);
    void reset();
    int get(std::string_view mapping, std::string_view path);

    static tokamap::SignalType deduce_signal_type(std::string_view final_path_element);
    static std::pair<std::vector<int>, std::deque<std::string>>
    extract_indices(const std::deque<std::string>& path_tokens);
    int add_machine_specific_attributes(nlohmann::json& attributes);
    static std::string generate_map_path(std::deque<std::string>& path_tokens, const std::vector<int>& indices,
                                         IDSMapRegister& mappings, const std::string& full_path);

private:
    // Loads, controls, stores mapping file lifetime
    std::unordered_map<std::string, std::string> _options;
    MappingHandler _mapping_handler;
    bool _init = false;
    std::string _request_function;
};

TokaMap::TokaMap(std::initializer_list<std::pair<std::string, std::string>> configuration) {
    std::unordered_map<std::string, std::string> options{configuration.begin(), configuration.end()};
    _impl = std::make_unique<TokaMapImpl>(options);

}

TokaMap::~TokaMap() = default;

void TokaMap::register_data_source(const std::string& name, std::unique_ptr<DataSource> data_source) {
    _data_sources.emplace(name, std::move(data_source));
}

tokamap::Data TokaMap::get_data(const std::string& path) {
    return {};
}

void TokaMap::load_mappings(std::string_view path) const {
    _impl->init(path);
}

/**
 * @brief Extract current indices and path tokens from IMAS/IDS path
 *
 * @param path_tokens deque of strings containing split IDS path tokens
 * @return {indices, processed_tokens} pair of the indices vector and tokens
 */
std::pair<std::vector<int>, std::deque<std::string>>
TokaMapImpl::extract_indices(const std::deque<std::string>& path_tokens)
{
    std::vector<int> indices;
    std::deque<std::string> processed_tokens;
    static const std::regex PATH_INDEX_RE{R"(\[\d+\])"};

    for (const auto& token : path_tokens) {
        std::sregex_token_iterator iter{ token.begin(), token.end(), PATH_INDEX_RE, 0 };
        std::sregex_token_iterator end;

        for (; iter != end; ++iter) {
            std::string num = iter->str().substr(1);
            indices.push_back(std::stoi(num));
        }
        const std::string new_token = std::regex_replace(token, PATH_INDEX_RE, "[#]");
        processed_tokens.push_back(new_token);
    }

    return {indices, processed_tokens};
}

/**
 * @brief Initialise the JSON_mapping_plugin
 *
 * Set mapping directory and load mapping files into mapping_handler
 * throw std::runtime_error if JSON mapping file location is not set
 *
 * @param path
 * @return errorcode UDA convention to return int errorcode
 * 0 success, !0 failure
 */
void TokaMapImpl::init(const std::string_view path)
{
    if (!_init || _request_function == "init" || _request_function == "initialise") {
        reset();
    }

    if (!path.empty()) {
        _mapping_handler.set_map_dir(path);
    } else {
        log(LogLevel::Error, "JSONMappingPlugin::init: - JSON mapping locations not set");
        throw std::runtime_error("JSONMappingPlugin::init: - JSON mapping locations not set");
    }
    _mapping_handler.init(false);
    _init = true;
}

/**
 * @brief
 *
 * @param plugin_interface Top-level UDA plugin interface
 * @return errorcode UDA convention to return int errorcode
 * 0 success, !0 failure
 */
void TokaMapImpl::reset()
{
    if (_init) {
        // Free Heap & reset counters if initialised
        _init = false;
    }
}

/**
 * @brief Deduce the type of signal being requested/mapped,
 * currently using string comparisons
 *
 * The final_path_element for error can be either be error_upper
 * or error_lower so search for substring error.
 *
 * @param final_path_element requested IDS path suffix (eg. data, time, error).
 * @note if no string is supplied, SignalType set to invalid
 * @return SignalType Enum class containing the current signal type
 * [DEFAULT, INVALID, DATA, TIME, ERROR]
 */
SignalType TokaMapImpl::deduce_signal_type(const std::string_view final_path_element)
{

    // SignalType useful in determining for MAST-U
    SignalType sig_type{SignalType::DEFAULT};
    if (final_path_element.empty()) {
        log(LogLevel::Debug, "Empty element suffix\n");
        sig_type = SignalType::INVALID;
    } else if (final_path_element == "data") {
        sig_type = SignalType::DATA;
    } else if (final_path_element == "time") {
        sig_type = SignalType::TIME;
    } else if (final_path_element.find("error") != std::string::npos) {
        sig_type = SignalType::ERROR;
    }
    return sig_type;
}

int TokaMapImpl::add_machine_specific_attributes(nlohmann::json& attributes)
{
    for (const auto& [name, value] : _options) {
        char* p_end = nullptr;
        long const i_value = std::strtol(value.c_str(), &p_end, 10);
        if (*p_end == '\0') {
            attributes[name] = i_value;
        } else {
            attributes[name] = value;
        }
    }

    return 0;
}

std::string TokaMapImpl::generate_map_path(std::deque<std::string>& path_tokens, const std::vector<int>& indices,
                                                 IDSMapRegister& mappings, const std::string& full_path)
{
    const auto sig_type = deduce_signal_type(path_tokens.back());
    if (sig_type == SignalType::INVALID) {
        return {}; // Don't throw, go gentle into that good night
    }

    std::string map_path = boost::algorithm::join(path_tokens, "/");
    log(LogLevel::Info, map_path);

    std::string found_path;

    if (mappings.count(map_path) == 0) {
        if (sig_type == SignalType::TIME or sig_type == SignalType::DATA) {
            path_tokens.pop_back();
            map_path = boost::algorithm::join(path_tokens, "/");
        }
        found_path = find_mapping(mappings, map_path, indices, full_path);
    } else {
        found_path = map_path;
    }

    return found_path;
}

/**
 * @brief Main data/mapping function called from class entry function
 *
 * Arguments:
 *  - machine   string      the name of the machine to map data for
 *  - path      string      the IDS path we need to map data for
 *  - rank      int         the rank of the data expected
 *  - shape     int array   the shape of the data expected
 *  - datatype  UDA_TYPE    the type of the data expected
 *  - <machine specific args> any remaining arguments are specific to the machine and have been passed via query
 *    arguments on the URI, i.e. imas://server/uda?machine=MASTU&shot=30420&run=1 would pass shot and run as additional
 *    arguments
 *
 * @param mapping
 * @param path
 * @return UDA convention to return int error code (0 success, !0 failure)
 */
int TokaMapImpl::get(const std::string_view mapping, const std::string_view path)
{
    std::deque<std::string> path_tokens;
    boost::split(path_tokens, path, boost::is_any_of("/"));
    if (path_tokens.empty()) {
        log(LogLevel::Error, "JSONMappingPlugin::get: - IDS path could not be split");
        throw std::runtime_error("JSONMappingPlugin::get: - IDS path could not be split");
    }

    std::vector<int> indices;
    std::tie(indices, path_tokens) = extract_indices(path_tokens);

    // Use first hash of the IDS path as the IDS name
    std::string const ids_name{path_tokens.front()};

    // Use lowercase machine name for find mapping files
    std::string machine_string = mapping.data();
    boost::to_lower(machine_string);

    const auto shot = _options.count("shot") != 0
        ? std::stoi(_options.at("shot"))
        : std::optional<int>{};

    // Load mappings based off IDS name
    // Returns a reference to IDS map objects and corresponding globals
    // Mapping object lifetime owned by mapping_handler
    const auto maybe_mappings = _mapping_handler.read_mappings(machine_string, ids_name, shot);

    if (!maybe_mappings) {
        log(LogLevel::Error, "JSONMappingPlugin::get: - JSON mapping not loaded, no map entries");
        throw std::runtime_error("JSONMappingPlugin::get: - JSON mapping not loaded, no map entries");
    }

    const auto& [attributes, mappings] = maybe_mappings.value();

    // Remove IDS name from path and rejoin for hash map key
    // magnetics/coil/#/current -> coil/#/current
    path_tokens.pop_front();

    const auto sig_type = deduce_signal_type(path_tokens.back());
    std::string const map_path = generate_map_path(path_tokens, indices, mappings, path.data());
    if (map_path.empty()) {
        return 1; // No mapping found, don't throw
    }

    // Add request indices to globals
    attributes["indices"] = indices;

    add_machine_specific_attributes(attributes);

    MapArguments const map_arguments{mappings, attributes, sig_type};

    return mappings.at(map_path)->map(map_arguments);
}
