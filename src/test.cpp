#include "library.h"

class TestDataSource final : public DataSource {
    std::unique_ptr<tokamap::Data> get_data(std::string_view path) override {
        return {};
    }
};

int main() {
    TokaMap map{
        {"config", "option"}
    };
    map.register_data_source("test", std::make_unique<TestDataSource>());
    map.load_mappings("mappings");
    tokamap::Data result = map.get_data("/magnetics/flux_loop[0]/flux/data");
}
