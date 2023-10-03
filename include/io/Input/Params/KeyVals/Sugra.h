#include <io/Input/Params/KeyVals/Modify.h>

class Sugra : public Modify
{
private:
    std::map<std::string, double&> doubleMap(ModelBase& modelParams) override;
    std::map<std::string, int&> intMap(ModelBase& modelParams) override;
    std::map<std::string, bool&> boolMap(ModelBase& modelParams) override;
public:
    Sugra(Logger& logger);
    ~Sugra();
};
