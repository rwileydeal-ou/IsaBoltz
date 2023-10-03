#include <io/Logging/Logger.h>

using namespace std;
namespace logging = boost::log;

Logger::Logger(){
    boost::log::sources::severity_logger< logging::trivial::severity_level > lg;
    _logger = lg;
}
Logger::Logger(std::string appenderName){
    Logger();
}
Logger::~Logger(){}

void Logger::AddLogFile(std::string logPath, std::string logPrefix){
    string filename = logPath + logPrefix + ".log";
    logging::add_file_log(filename);
}

void Logger::SetPriority(LoggingLevel logLevel){
    auto targetLevel = logging::trivial::debug;
	switch (logLevel)
    {
    case LoggingLevel::TRACE:
        targetLevel = logging::trivial::trace;
        break;
    case LoggingLevel::DEBUG:
        targetLevel = logging::trivial::debug;
        break;
    case LoggingLevel::INFO:
        targetLevel = logging::trivial::info;
        break;
    case LoggingLevel::WARN:
        targetLevel = logging::trivial::warning;
        break;
    case LoggingLevel::ERROR:
        targetLevel = logging::trivial::error;
        break;
    case LoggingLevel::FATAL:
        targetLevel = logging::trivial::fatal;
        break;    
    default:
        throw logic_error("Select valid logging level!");
    }
    logging::core::get()->set_filter(
        logging::trivial::severity >= targetLevel
    );
}

void Logger::Fatal(string msg){
    BOOST_LOG_SEV(_logger, logging::trivial::fatal) << msg;
}

void Logger::Error(string msg){
    BOOST_LOG_SEV(_logger, logging::trivial::error) << msg;
}

void Logger::Warn(string msg){
    BOOST_LOG_SEV(_logger, logging::trivial::warning) << msg;
}

void Logger::Info(string msg){
    BOOST_LOG_SEV(_logger, logging::trivial::info) << msg;
}

void Logger::Debug(string msg){
    BOOST_LOG_SEV(_logger, logging::trivial::debug) << msg;
}

void Logger::Trace(string msg){
    BOOST_LOG_SEV(_logger, logging::trivial::trace) << msg;
}
