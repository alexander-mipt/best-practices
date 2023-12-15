#ifndef JIT_AOT_COMPILERS_COURSE_LOGGER_H_
#define JIT_AOT_COMPILERS_COURSE_LOGGER_H_

#include <log4cpp/Category.hh>
#include <log4cpp/OstreamAppender.hh>
#include "macros.h"
#include <memory>


namespace utils {
using LogPriority = log4cpp::Priority;

// simple wrapper to store reference to logger
class Logger {
public:
    Logger(log4cpp::Category &log) : logger(log) {
        initLogger();
    }
    NO_COPY_SEMANTIC(Logger);
    NO_MOVE_SEMANTIC(Logger);
    virtual DEFAULT_DTOR(Logger);

    log4cpp::Category &GetLoggerRaw() {
        return logger;
    }
    log4cpp::CategoryStream GetLogger(LogPriority::PriorityLevel p) const {
        return logger << p;
    }

    static log4cpp::Category &GetRoot() {
        auto &root = log4cpp::Category::getRoot();
        if (root.getAllAppenders().empty()) {
            ASSERT(log4cpp::Appender::getAppender(STDERR_APPENDER_NAME) == nullptr);
            auto *appender = new log4cpp::OstreamAppender(STDERR_APPENDER_NAME, &std::cerr);
            appender->setLayout(new log4cpp::BasicLayout());
            root.addAppender(appender);
        }
        return root;
    }

public:
    static constexpr const char *STDOUT_APPENDER_NAME = "stdout";
    static constexpr const char *STDERR_APPENDER_NAME = "stderr";

protected:
    static log4cpp::Appender &GetStdOutAppender() {
        // TODO: properly init global appender
        if UNLIKELY(stdoutAppender == nullptr) {
            stdoutAppender = std::make_unique<log4cpp::OstreamAppender>(
                STDOUT_APPENDER_NAME, &std::cout);
            stdoutAppender->setLayout(new log4cpp::BasicLayout());
        }
        return *stdoutAppender;
    }

    virtual void initLogger() const {
        if (logger.getAllAppenders().empty()) {
            logger.addAppender(GetStdOutAppender());
        }
    }

private:
    static std::unique_ptr<log4cpp::OstreamAppender> stdoutAppender;

    log4cpp::Category &logger;
};
}   // namespace utils

#endif  // JIT_AOT_COMPILERS_COURSE_LOGGER_H_
