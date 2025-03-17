#include "utils/scale_offset.hpp"

#include "logging.hpp"

int tokamap::map_transform::transform_offset(const Data* data_block, const float offset)
{
    int err{1};
    if (data_block->rank > 0) {
        const size_t array_size(data_block->data_n);
        switch (data_block->data_type) {
            case DataType::Int8: {
                auto* data = reinterpret_cast<short*>(data_block->data.get());
                err = offset_span(gsl::span{data, array_size}, offset);
                break;
            }
            case DataType::Int32: {
                auto* data = reinterpret_cast<int*>(data_block->data.get());
                err = offset_span(gsl::span{data, array_size}, offset);
                break;
            }
            case DataType::Int64: {
                auto* data = reinterpret_cast<long*>(data_block->data.get());
                err = offset_span(gsl::span{data, array_size}, offset);
                break;
            }
            case DataType::Float: {
                auto* data = reinterpret_cast<float*>(data_block->data.get());
                err = offset_span(gsl::span{data, array_size}, offset);
                break;
            }
            case DataType::Double: {
                auto* data = reinterpret_cast<double*>(data_block->data.get());
                err = offset_span(gsl::span{data, array_size}, offset);
                break;
            }
            default:
                log(LogLevel::Debug, "\nOffsetEntry::transform(...) Unrecognised type\n");
                return 1;
        }
    } else {
        switch (data_block->data_type) {
            case DataType::Int8: {
                auto* data = reinterpret_cast<short*>(data_block->data.get());
                err = offset_value(data, offset);
                break;
            }
            case DataType::Int32: {
                auto* data = reinterpret_cast<int*>(data_block->data.get());
                err = offset_value(data, offset);
                break;
            }
            case DataType::Int64: {
                auto* data = reinterpret_cast<long*>(data_block->data.get());
                err = offset_value(data, offset);
                break;
            }
            case DataType::Float: {
                auto* data = reinterpret_cast<float*>(data_block->data.get());
                err = offset_value(data, offset);
                break;
            }
            case DataType::Double: {
                auto* data = reinterpret_cast<double*>(data_block->data.get());
                err = offset_value(data, offset);
                break;
            }
            default:
                log(LogLevel::Debug, "\nOffsetEntry::transform(...) Unrecognised type\n");
                break;
        }
    }

    return err;
}

int tokamap::map_transform::transform_scale(const Data* data_block, const float scale)
{
    int err{1};
    if (data_block->rank > 0) {
        const size_t array_size(data_block->data_n);
        switch (data_block->data_type) {
            case DataType::Int8: {
                auto* data = reinterpret_cast<short*>(data_block->data.get());
                err = scale_span(gsl::span{data, array_size}, scale);
                break;
            }
            case DataType::Int32: {
                auto* data = reinterpret_cast<int*>(data_block->data.get());
                err = scale_span(gsl::span{data, array_size}, scale);
                break;
            }
            case DataType::Int64: {
                auto* data = reinterpret_cast<long*>(data_block->data.get());
                err = scale_span(gsl::span{data, array_size}, scale);
                break;
            }
            case DataType::Float: {
                auto* data = reinterpret_cast<float*>(data_block->data.get());
                err = scale_span(gsl::span{data, array_size}, scale);
                break;
            }
            case DataType::Double: {
                auto* data = reinterpret_cast<double*>(data_block->data.get());
                err = scale_span(gsl::span{data, array_size}, scale);
                break;
            }
            default:
                log(LogLevel::Debug, "\nOffsetEntry::transform(...) Unrecognised type\n");
                return 1;
        }
    } else {
        switch (data_block->data_type) {
            case DataType::Int8: {
                auto* data = reinterpret_cast<short*>(data_block->data.get());
                err = scale_value(data, scale);
                break;
            }
            case DataType::Int32: {
                auto* data = reinterpret_cast<int*>(data_block->data.get());
                err = scale_value(data, scale);
                break;
            }
            case DataType::Int64: {
                auto* data = reinterpret_cast<long*>(data_block->data.get());
                err = scale_value(data, scale);
                break;
            }
            case DataType::Float: {
                auto* data = reinterpret_cast<float*>(data_block->data.get());
                err = scale_value(data, scale);
                break;
            }
            case DataType::Double: {
                auto* data = reinterpret_cast<double*>(data_block->data.get());
                err = scale_value(data, scale);
                break;
            }
            default:
                log(LogLevel::Debug, "\nOffsetEntry::transform(...) Unrecognised type\n");
                break;
        }
    }

    return err;
}
