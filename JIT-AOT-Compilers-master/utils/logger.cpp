#include "logger.h"


namespace utils {
std::unique_ptr<log4cpp::OstreamAppender> Logger::stdoutAppender = nullptr;
}   // namespace utils
