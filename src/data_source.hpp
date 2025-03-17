#pragma once

struct DataSource {
    virtual ~DataSource() = default;

    virtual std::unique_ptr<tokamap::Data> get_data(std::string_view path) = 0;
};