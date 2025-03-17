#pragma once

#include <string>
#include <sstream>

#include "data.hpp"

namespace tokamap
{
    template<typename T>
    std::string print_typed_buffer(const T* data, const int max_size)
    {
        std::stringstream ss;
        ss << "[";
        int max_elements = std::min(max_size, 10);
        const bool all_data =  max_elements == max_size;
        if (!all_data)
        {
            max_elements /= 2;
        }
        for (unsigned int i = 0; i < max_elements; ++i)
        {
           ss << data[i] << ","; 
        }
        if (!all_data)
        {
            ss << " ... ";
            for (unsigned int i = max_size-max_elements-1; i < max_size; ++i)
            {
                ss << data[i] << ","; 
            } 
        }
        ss << "]" << std::endl;
        return ss.str();
    }

    inline std::string print_uda_data_buffer(const char* data, const DataType data_type, const int max_size)
    {
        switch (data_type)
        {
            case DataType::Int32:
                return print_typed_buffer(reinterpret_cast<const int*>(data), max_size);
            case DataType::Int8:
                return print_typed_buffer(reinterpret_cast<const short*>(data), max_size);
            case DataType::Int64:
                return print_typed_buffer(reinterpret_cast<const long*>(data), max_size);
            case DataType::Float:
                return print_typed_buffer(reinterpret_cast<const float*>(data), max_size);
            case DataType::Double:
                return print_typed_buffer(reinterpret_cast<const double*>(data), max_size);
            default:
                throw std::runtime_error("unknown type " + to_string(data_type));
        }
    }

    inline std::string print_data_block(const Data& data_block)
    {
        std::stringstream ss;
        ss <<  "data_number  : " << data_block.data_n << std::endl;
        ss <<  "rank         : " << data_block.rank << std::endl;
        ss <<  "data_type    : " << static_cast<int>(data_block.data_type) << std::endl;
        return ss.str();
    }

}
