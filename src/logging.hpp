#pragma once

#include <string_view>

enum class LogLevel { Debug, Info, Warning, Error };

int log(LogLevel log_level, std::string_view log_msg);