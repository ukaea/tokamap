#include "logging.hpp"

#include <string>
#include <fstream>

/**
 * @brief Temporary logging function for JSON_mapping_plugin, outputs
 * to UDA_HOME/etc/
 *
 * @param log_level The JPLogLevel (INFO, WARNING, ERROR, DEBUG)
 * @param log_msg The message to be logged
 * @return
 */
int log(const LogLevel log_level, const std::string_view log_msg)
{
    // std::string const log_file_name = std::string{static_cast<const char*>(environment->logdir)} + "/JSON_plugin.log";
    const std::string log_file_name = "./tokamap.log";
    std::ofstream jp_log_file;
    jp_log_file.open(log_file_name, std::ios_base::out | std::ios_base::app);
    std::time_t const time_now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    const auto timestamp = std::put_time(std::gmtime(&time_now), "%Y-%m-%d:%H:%M:%S"); // NOLINT(concurrency-mt-unsafe)
    if (!jp_log_file) {
        return 1;
    }

    switch (log_level) {
        case LogLevel::Debug:
            jp_log_file << timestamp << ":DEBUG - ";
        break;
        case LogLevel::Info:
            jp_log_file << timestamp << ":INFO - ";
        break;
        case LogLevel::Warning:
            jp_log_file << timestamp << ":WARNING - ";
        break;
        case LogLevel::Error:
            jp_log_file << timestamp << ":ERROR - ";
        break;
        default:
            jp_log_file << "LOG_LEVEL NOT DEFINED";
    }
    jp_log_file << log_msg << "\n";
    jp_log_file.close();

    return 0;
}