#pragma once

#include <string>

namespace tokamap {

enum class DataType : int {
    Unknown,
    Int8,
    Int16,
    Int32,
    Int64,
    UInt8,
    UInt16,
    UInt32,
    UInt64,
    Float,
    Double,
    String,
};

constexpr int DataTypeMin = static_cast<int>(DataType::Unknown);
constexpr int DataTypeMax = static_cast<int>(DataType::String);

inline std::string to_string(const DataType type) {
    switch (type) {
        case DataType::Int8: return "int8";
        case DataType::Int16: return "int16";
        case DataType::Int32: return "int32";
        case DataType::Int64: return "int64";
        case DataType::UInt8: return "uint8";
        case DataType::UInt16: return "uint16";
        case DataType::UInt32: return "uint32";
        case DataType::UInt64: return "uint64";
        case DataType::Float: return "float";
        case DataType::String: return "string";
        default: return "unknown";
    }
}

struct Data {
    std::unique_ptr<std::byte[]> data;
    DataType data_type;
    int rank;
    size_t data_n;
};

} // namespace tokamap