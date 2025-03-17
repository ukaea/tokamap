#pragma once

#include <cstring>
#include <numeric>
#include <typeinfo>
#include <unordered_map>
#include <valarray>
#include <vector>
#include <boost/mpl/aux_/na_fwd.hpp>
#include <exprtk/exprtk.hpp>

#include <gsl/gsl-lite.hpp>

#include "data.hpp"

namespace tokamap::helpers
{

inline std::unordered_map<std::string, DataType> type_map()
{
    static std::unordered_map<std::string, DataType> _type_map;
    if (_type_map.empty()) {
        _type_map = {{typeid(unsigned int).name(), DataType::UInt32},
                    {typeid(int).name(), DataType::Int32},
                    {typeid(float).name(), DataType::Float},
                    {typeid(double).name(), DataType::Double}};
    }
    return _type_map;
}

template <typename T>
void set_return_data_scalar_type(Data* data_block, T value)
{
    const auto* bytes = reinterpret_cast<std::byte*>(&value);
    const size_t n_bytes = sizeof(T);

    data_block->rank = 0;
    data_block->data_type = type_map().at(typeid(T).name());
    data_block->data = std::make_unique<std::byte[]>(n_bytes);
    std::copy_n(bytes, n_bytes, data_block->data.get());
    data_block->data_n = 1;
}

template <typename T>
void set_return_data_array_type(Data* data_block, const gsl::span<const T> values, const int rank)
{
    const auto* bytes = reinterpret_cast<std::byte*>(values.data());
    const size_t n_bytes = values.size() * sizeof(T);

    data_block->rank = rank;
    data_block->data_type = type_map().at(typeid(T).name());
    data_block->data = std::make_unique<std::byte[]>(n_bytes);
    std::copy_n(bytes, n_bytes, data_block->data.get());
    data_block->data_n = values.size();
}

template <typename T>
void set_return_data_array_vec(Data* data_block, const std::vector<T>& values)
{
    const auto* bytes = reinterpret_cast<std::byte*>(values.data());
    const size_t n_bytes = values.size() * sizeof(T);

    data_block->rank = 1;
    data_block->data_type = type_map().at(typeid(T).name());
    data_block->data = std::make_unique<std::byte[]>(n_bytes);
    std::copy_n(bytes, n_bytes, data_block->data.get());
    data_block->data_n = values.size();
}

template <typename T>
void set_return_data_val_array(Data* data_block, const std::valarray<T>& values)
{
    const auto* bytes = reinterpret_cast<std::byte*>(values.data());
    const size_t n_bytes = values.size() * sizeof(T);

    data_block->rank = 1;
    data_block->data_type = type_map().at(typeid(T).name());
    data_block->data = std::make_unique<std::byte[]>(n_bytes);
    std::copy_n(bytes, n_bytes, data_block->data.get());
    data_block->data_n = values.size();
}

} // namespace tokamap::helpers
