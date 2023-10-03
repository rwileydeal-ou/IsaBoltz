#include <io/FileIO.h>
#include <io/Input/FileParser.h>
#include <io/Input/Run/RunParams.h>
#include <io/Logging/Logger.h>
#include <map>

class RunCardParser : public FileParser
{
    private:
        Logger& logger_;
        LoggingLevel parseLogLevel(std::string line);
        std::string parseString(std::string line);
        RunParams Convert(std::string infile);
    public:
        RunCardParser(Logger& logger);
        ~RunCardParser();
        RunParams Run;
        void Parse(std::string runCard);
};
