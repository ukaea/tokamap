#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "data.hpp"
#include "data_source.hpp"

class TokaMapImpl;

class TokaMap {
public:
    TokaMap(std::initializer_list<std::pair<std::string, std::string>> configuration);
    ~TokaMap();

    void load_mappings(std::string_view path) const;
    void register_data_source(const std::string& name, std::unique_ptr<DataSource> data_source);
    tokamap::Data get_data(const std::string& path);

private:
    std::unordered_map<std::string, std::unique_ptr<DataSource>> _data_sources;
    std::unique_ptr<TokaMapImpl> _impl;
};

