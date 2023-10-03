#ifndef Macro_h
#define Macro_h

#include <string>
#include <vector>
#include <iostream>
#include <interface/AutoSuggestions.h>
#include <io/FileIO.h>
#include <cmd/Commands.h>

class Macro
{
private:
    bool interactiveMode_;
protected:
    std::vector<std::string> enabledParticleKeys( std::vector<std::string> supportedParticleKeys, CommandWithPayload cmd, std::string cmdFriendlyName );
public:
    Macro(bool interactiveMode);
    ~Macro();
    static std::vector<std::string> cmdInput(std::vector<std::string> supportedOptions);
    virtual void Execute() = 0;
};

#endif
