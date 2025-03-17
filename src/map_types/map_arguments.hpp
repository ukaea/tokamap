#pragma once

#include <unordered_map>
#include <nlohmann/json.hpp>

#include "data.hpp"

namespace tokamap {
enum class SignalType { Default, Data, Time, Error, Dim, Invalid };

class Mapping;

struct MapArguments {
    const std::unordered_map<std::string, std::unique_ptr<Mapping>>& _entries;
    const nlohmann::json& _global_data;
    SignalType _sig_type;
    DataType _datatype;
    int _rank;

    explicit MapArguments(const std::unordered_map<std::string, std::string>& options,
                          const std::unordered_map<std::string, std::unique_ptr<Mapping>>& entries,
                          const nlohmann::json& global_data, const SignalType sig_type)
        : _entries{entries}
        , _global_data{global_data}
        , _sig_type{sig_type}
        , _datatype{DataType::Unknown}
        , _rank{-1}
    {
        extract_interface_arguments(options);
    }

private:
    int extract_interface_arguments(const std::unordered_map<std::string, std::string>& options)
    {
        if (options.count("datatype") != 0) {
            throw std::invalid_argument("datatype required");
        }

        int type = std::stoi(options.at("datatype"));
        if (type < DataTypeMin || type > DataTypeMin) {
            throw std::invalid_argument("invalid datatype provided");
        }

        _datatype = static_cast<DataType>(type);

        if (options.count("rank") != 0) {
            throw std::invalid_argument("rank required");
        }

        _rank = std::stoi(options.at("rank"));

        return 0;
    }
};

} // namespace tokamap