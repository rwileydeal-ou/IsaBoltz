#ifndef Commands_h
#define Commands_h

#include <string>

enum SupportedCommands{
    // launch must be kept as the first valuee
    // commands get sorted s.t. launch is the last cmd per macro
    // used to relay message that invoker should execute the set of commands
    LAUNCH,
    SPECTRUM,
    BRANCHING_RATIO,
    SOLVE_BOLTZMANN,
    CROSS_SECTION,
    TEMP_DECAY,
    TEMP_OSC,
    TEMP_DECOUPLE,
    TEMP_EQUALITY,
    CRITICAL_ABUNDANCE,
    FREEZEOUT_ABUNDANCE,
    DELTA_NEFF,
    // parameter_override must be kept as the final/second to final value
    // commands get sorted s.t. the parameter overrides are the first things that get handled
    // only exit should take precedence
    PARAMETER_OVERRIDE,
    EXIT
};

struct CommandWithPayload{
    SupportedCommands Command;
    std::string Payload;

    CommandWithPayload(){

    };
    CommandWithPayload(SupportedCommands cmd, std::string payload){
        Command = cmd;
        Payload = payload;
    };
};

#endif
