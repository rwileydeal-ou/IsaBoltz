#ifndef IsaBoltzLogger_h
#define IsaBoltzLogger_h

#include <iostream>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <io/Logging/LoggingLevel.h>
#include <boost/stacktrace.hpp>
#include <boost/exception/all.hpp>

typedef boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace> traced;

class Logger
{
private:
    boost::log::sources::severity_logger< boost::log::trivial::severity_level > _logger;
public:
    void Fatal(std::string msg);
    void Error(std::string msg);
    void Warn(std::string msg);
    void Notice(std::string msg);
    void Info(std::string msg);
    void Debug(std::string msg);
    void Trace(std::string msg);
    void SetPriority(LoggingLevel logLevel);
    void AddLogFile(std::string logPath, std::string logPrefix);
    Logger();
    Logger(std::string appenderName);
    ~Logger();
};

template <class E>
void throw_with_trace(const E& e) {
    throw boost::enable_error_info(e)
        << traced(boost::stacktrace::stacktrace());
}

#endif
