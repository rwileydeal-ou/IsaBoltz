#include <Models/ModelBase.h>
#include <io/Logging/Logger.h>
#include <io/Input/FileParser.h>

class Mass
{
private:
    Logger& logger_;
public:
    void setKeyVal(std::string key, std::string keyVal, ModelBase& modelParams);
    Mass(Logger& logger);
    ~Mass();
};
