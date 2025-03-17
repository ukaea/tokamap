#pragma once

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <string>
#include <deque>
#include <set>

#include "utils/print_uda_structs.hpp"
#include "logging.hpp"
#include "data.hpp"

/*
 *
 * NOTES:
 *
 * using char instead of byte to avoid extra casting from/to datablock
 *
 */

namespace tokamap
{

inline void log_datablock_status(const Data& data_block, const std::string& message)
{
    log(LogLevel::Debug, message + "\n" + print_data_block(data_block));
}

static constexpr uint32_t default_size = 100;
static constexpr uint32_t default_drop = 10;

class RamCache
{
  public:
    RamCache() = default;

    explicit RamCache(const bool cache_logging, const uint32_t max_items, const uint32_t drop_items)
        : _logging_active(cache_logging)
        , _max_items(max_items)
        , _drop_items(drop_items)
    {
    }

    void add(const std::string& key, std::unique_ptr<Data> value)
    {
        if (has_entry(key)) {
            throw std::runtime_error("key already exists");
        }
        if (_items.size() == _max_items) {
            drop_items();
        }
        _keys.insert(key);
        _items.emplace_front(key, std::move(value));
        log(LogLevel::Info, "entry added to cache: \"" + key + "\". cache size is now " +
                                std::to_string(_items.size()) + " / " + std::to_string(_max_items));
    }

    [[nodiscard]] bool has_entry(const std::string& key) const {
        return _keys.count(key) != 0;
    }

    std::optional<Data> copy_from_cache(const std::string& key);

  private:
    using ClockType = std::chrono::time_point<std::chrono::system_clock>;
    using DataEntry = std::pair<ClockType, std::unique_ptr<Data>>;
    using CacheEntry = std::pair<std::string, std::unique_ptr<Data>>;

    bool _logging_active = false;
    uint32_t _max_items = default_size;
    uint32_t _drop_items = default_drop;

    std::set<std::string> _keys;
    std::deque<CacheEntry> _items;

    void drop_items() {
        for (int i = 0; i < _drop_items; i++) {
            auto& name = _items.back().first;
            _keys.erase(name);
            _items.pop_back();
        }
    }
};

} // namespace ram_cache
