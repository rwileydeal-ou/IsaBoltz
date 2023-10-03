#ifndef KeyValModify_h
#define KeyValModify_h

#include <Models/ModelBase.h>
#include <io/Logging/Logger.h>
#include <io/Input/FileParser.h>

class Modify
{
private:
    Logger& logger_;
    virtual std::map<std::string, double&> doubleMap(ModelBase& model) = 0;
    virtual std::map<std::string, int&> intMap(ModelBase& model) = 0;
    virtual std::map<std::string, bool&> boolMap(ModelBase& model) = 0;
public:
    void SetKeyVal(std::string key, std::string keyVal, ModelBase& model);
    Modify(Logger& logger);
    ~Modify();
};

#endif
