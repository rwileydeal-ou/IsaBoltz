#ifndef FileIO_h
#define FileIO_h

#include <iostream> 
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <io/Logging/Logger.h>

struct FileNotFoundException : public std::exception{
    const char * what () const throw(){
        return "File not found.";
    }
};

class FileIO{
    public:
        FileIO();
        ~FileIO();
        static bool PathExists(std::string filePath, Logger& logger);
        static void ValidateFilePath(std::string path, Logger& logger);
        static void CreateDirectory(std::string path, Logger& logger);
        static void CreateDirectory(boost::filesystem::path path, Logger& logger);
        static bool LineContains(std::string substring, std::string line, bool caseSensitive=false);
        static std::vector<std::string> Split(const std::string& str, const std::string& delims);
        static std::string RunDirectory(std::string basePath, std::string directoryPrefix, std::string delimiter, Logger& logger);
};

#endif
