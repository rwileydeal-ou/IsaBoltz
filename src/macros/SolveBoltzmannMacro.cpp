#include <macros/SolveBoltzmannMacro.h>

SolveBoltzmannMacro::SolveBoltzmannMacro(CommandWithPayload cmd, std::shared_ptr< Sender > invoker, std::shared_ptr< MssmSpectrumCommand >& spectraCmd, bool interactiveMode, Connection& connection) :
    Macro(interactiveMode),
    connection_(connection)
{
    if (!spectraCmd){
        spectraCmd = std::make_shared< MssmSpectrumCommand >(connection_);
        invoker -> AddCommand( spectraCmd );
    }

    cmd_ = cmd;
    invoker_ = invoker;
    interactiveMode_ = interactiveMode;
    reheatPoint_ = std::make_shared< Models::ScaleFactorPoint >();
    spectraCmd_ = spectraCmd;
}

SolveBoltzmannMacro::~SolveBoltzmannMacro(){
}

void SolveBoltzmannMacro::setInitialConditions(const std::vector<std::string>& enabledKeys, std::shared_ptr< DataRelay >& fortranInterface){
    // for now, we're just starting the initial conditions at reheat
    // TODO: configure with possibly inflationary scenario, or other initial condition options(?)
    BoltzmannInitialConditionsMacro initialConditions(
        cmd_, 
        invoker_,
        enabledKeys,
        reheatPoint_,
        interactiveMode_,
        fortranInterface,
        connection_
    );

    initialConditions.Execute();
}

void SolveBoltzmannMacro::solveBoltzmannEqns(const std::vector<std::string>& enabledKeys, std::shared_ptr< DataRelay >& fortranInterface){
    double finalTemp = connection_.Model.Cosmology.Temperatures.Final;
    BoltzmannSolverCommand solveCmd(
        connection_, 
        fortranInterface,
        reheatPoint_ -> Id, 
        finalTemp, 
        enabledKeys
    );
    
    solveCmd.Execute();
}

void SolveBoltzmannMacro::Execute(){
    std::vector<std::string> supportedKeys = { "neutralino1", "gravitino", "axino", "cohosc_saxion", "thermal_saxion", "cohosc_axion", "thermal_axion", "cohosc_modulus" };
    std::vector<std::string> enabledKeys = enabledParticleKeys(supportedKeys, cmd_, "Boltzmann");

    auto fortranInterface = spectraCmd_ -> getFortranInterface();

    setInitialConditions(enabledKeys, fortranInterface);
    solveBoltzmannEqns(enabledKeys, fortranInterface);

    for ( auto& key : enabledKeys ){
        RelicDensityCommand oh2Cmd(
            connection_, 
            key 
        );
        oh2Cmd.Execute();

        DeltaNeffCommand dneffCmd(
            connection_,
            key
        );
        dneffCmd.Execute();

        CheckBBNCommand checkBBNCmd(
            connection_,
            key
        );
        checkBBNCmd.Execute();
    }
    RelicDensityCommand oh2RadCmd(
        connection_, 
        "photon"
    );
    oh2RadCmd.Execute();
}
