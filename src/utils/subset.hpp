#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

#include "utils/print_uda_structs.hpp"
#include "logging.hpp"

namespace tokamap::subset
{

class SubsetInfo
{
public:
    explicit SubsetInfo(const uint64_t size) : _start(0), _stop(size), _dim_size(size) {}

    SubsetInfo(const uint64_t start, const uint64_t stop, const int stride, const uint64_t size)
        : _start(start), _stop(stop), _stride(stride), _dim_size(size)
    {
        if (start == 0 and stop == 0) {
            _stop = size;
        };
    }

    [[nodiscard]] uint64_t size() const { return std::floor((_stop - _start) / _stride); }

    [[nodiscard]] bool validate() const { return _stop <= _dim_size and _stride < _dim_size; }

    [[nodiscard]] uint64_t start() const { return _start; }

    [[nodiscard]] uint64_t stop() const { return _stop; }

    [[nodiscard]] int64_t stride() const { return _stride; }

    [[nodiscard]] uint64_t dim_size() const { return _dim_size; }

    [[nodiscard]] std::string print_to_string() const
    {
        std::stringstream ss;
        ss << "start: " << _start << std::endl;
        ss << "stop: " << _stop << std::endl;
        ss << "stride: " << _stride << std::endl;
        ss << "dim_size: " << _dim_size << std::endl;
        return ss.str();
    }

private:
    uint64_t _start;
    uint64_t _stop;
    int64_t _stride = 1;
    uint64_t _dim_size;
};

void apply_subsetting(Data* data, double scale_factor, double offset);

template <typename T> void subset(Data* data, double scale_factor, double offset);

template <typename T>
std::vector<T> subset(std::vector<T>& input, std::vector<SubsetInfo>& subset_dims, double scale_factor = 1.0,
                      double offset = 0.0);

} // namespace subset
