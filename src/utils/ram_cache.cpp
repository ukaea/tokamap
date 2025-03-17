#include "ram_cache.hpp"

#include <stdexcept>

#include "utils/uda_plugin_helpers.hpp"

std::optional<tokamap::Data> tokamap::RamCache::copy_from_cache(const std::string& key)
{
    if (!has_entry(key)) {
        return {};
    }

    log(LogLevel::Info, "key found in ram-cache: \"" + key + "\". copying data out");

    const auto& result = std::find_if(_items.begin(), _items.end(),
        [&key](const CacheEntry& entry) { return entry.first == key; });
    if (result == _items.end()) {
        throw std::runtime_error("key not found in ram-cache");
    }

    const std::unique_ptr<Data>& data = result->second;

    Data data_block;
    data_block.data = std::make_unique<std::byte[]>(data->data_n);
    std::copy_n(data->data.get(), data->data_n, data_block.data.get());
    data_block.data_type = data->data_type;
    data_block.rank = data->rank;
    data_block.data_n = data->data_n;

    log_datablock_status(data_block, "data_block from cache");
    return { std::move(data_block) };
}
