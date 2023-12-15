#include "debug.h"
#include "logger.h"


namespace utils {
void AssertionFail(const char *expr, const char *file, unsigned line, const char *function) {
    Logger::GetRoot() << utils::LogPriority::CRIT << "ASSERTION FAILED: " << expr;
    Logger::GetRoot() << utils::LogPriority::CRIT << "IN " << file << ":" << std::dec << line << ":" << function;
    std::terminate();
}

void PrintWarning(const char *mess, const char *file, unsigned line, const char *function) {
    Logger::GetRoot() << utils::LogPriority::WARN << "WARNING: " << mess;
    Logger::GetRoot() << utils::LogPriority::WARN << "IN " << file << ":" << std::dec << line << ":" << function;
}

void AssertionFail(std::string expr, const char *file, unsigned line, const char *function) {
    AssertionFail(expr.data(), file, line, function);
}

void PrintWarning(std::string mess, const char *file, unsigned line, const char *function) {
    PrintWarning(mess.data(), file, line, function);
}
}   // namespace utils
