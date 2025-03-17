#include <stdexcept>

#include "utils/subset.hpp"
#include "utils/uda_type_sizes.hpp"
#include "logging.hpp"

template <typename T>
std::vector<T> tokamap::subset::subset(std::vector<T>& input, std::vector<SubsetInfo>& subset_dims, double scale_factor, double offset)
{
    log(LogLevel::Debug, "input size: " + std::to_string(input.size()));
    log(LogLevel::Debug, "input value 1: " + std::to_string(input[0]));
    log(LogLevel::Debug, "scaling factor is " + std::to_string(scale_factor));
    unsigned int result_length = 1;
    std::vector<unsigned int> total_dim_lengths;
    std::vector<unsigned int> current_indices;
    for (const auto& subset_info : subset_dims) {
        result_length *= subset_info.size();
        total_dim_lengths.emplace_back(subset_info.dim_size());
        current_indices.emplace_back(subset_info.start());
    }

    // std::vector<unsigned int> factors = get_index_factors(total_dim_lengths);
    log(LogLevel::Debug, "result length is: " + std::to_string(result_length));
    std::vector<T> result(result_length);
    for (unsigned int output_id = 0; output_id < result_length; ++output_id) {

        // increment vector of current_indices (cascading when they roll-over)
        for (unsigned int k = 0; k < subset_dims.size() and current_indices[k] >= subset_dims[k].stop(); ++k) {
            current_indices[k] = subset_dims[k].start();
            if (k < subset_dims.size() - 1) {
                current_indices[k + 1] += subset_dims[k + 1].stride();
            } else {
                // something wrong !
                throw std::runtime_error("unknown error encountered in subset function");
            }
        }
        // unsigned int input_id = get_input_offset(current_indices, factors);
        // result[output_id] = (input[input_id] * scale_factor) + offset;

        current_indices[0] += subset_dims[0].stride();
    }
    return result;
}

template <typename T>
void tokamap::subset::subset(Data* data, double scale_factor, double offset)
{

}

void tokamap::subset::apply_subsetting(Data* data, double scale_factor, double offset)
{
    log(LogLevel::Debug, "Entering apply subsetting function");
    if (data->rank == 0)
        return;
    switch (data->data_type) {
        case DataType::Int8:
            subset<short>(data, scale_factor, offset);
            break;
        case DataType::Int32:
            subset<int>(data, scale_factor, offset);
            break;
        case DataType::Int64:
            subset<long>(data, scale_factor, offset);
            break;
        case DataType::Float:
            subset<float>(data, scale_factor, offset);
            break;
        case DataType::Double:
            subset<double>(data, scale_factor, offset);
            break;
        default:
            throw std::runtime_error(std::string("type ") + to_string(data->data_type) +
                                     " not implemented for tokamap subset");
    }
}
