#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>

#include "data.hpp"

namespace tokamap::utils
{

inline size_t size_of(const DataType type_enum)
{
    switch(type_enum)
    {
        case DataType::Int8:
            return sizeof(int8_t);
        case DataType::Int16:
            return sizeof(int16_t);
        case DataType::Int32:
            return sizeof(int32_t);
        case DataType::Int64:
            return sizeof(int64_t);
        case DataType::Float:
            return sizeof(float);
        case DataType::Double:
            return sizeof(double);
        default:
            throw std::runtime_error(std::string("type ") + to_string(type_enum) + " not implemented for size_of");
    }
}

} // namespace tokamap::utils
