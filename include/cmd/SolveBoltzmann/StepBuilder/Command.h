#ifndef BoltzmannStepBuilderCommand_h
#define BoltzmannStepBuilderCommand_h

#include <cmath>
#include <math.h>
#include <map>
#include <chrono>
#include <future>
#include <thread>
#include <cctype>
#include <iostream>
#include <deque>
#include <vector>
#include <memory>
#include <boost/numeric/odeint.hpp>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include <boost/pool/pool_alloc.hpp>

#include <client/Sender.h>
#include <cmd/ICommand.h>
#include <io/Exceptions/NotImplemented.h>
#include <io/FileIO.h>
#include <Models/ScaleFactor.h>
#include <Models/ParticleEvolution.h>
#include <Models/TotalWidth.h>
#include <Models/Connection.h>
#include <Models/TempOsc.h>
#include <cmd/SolveBoltzmann/StepInfo.h>
#include <cmd/CrossSections/Command.h>
#include <cmd/BranchingRatios/Command.h>
#include <cmd/IsaTools/Command.h>
#include <cmd/SolveBoltzmann/InitialConditions/InitialField/CohOscMatter/Receiver.h>
#include <cmd/SolveBoltzmann/StepBuilder/SqlPostData.h>
#include <ModelIndependent/GStar.h>

#include <ModelIndependent/Densities/NumberDensityEq.h>
#include <cmd/SolveBoltzmann/StepBuilder/BoltzmannBuilder.h>

#include <sql/DbManager.h>
#include <sql/Statements/ScaleFactor.h>
#include <sql/Filters/ScaleFactor.h>
#include <sql/Callbacks/ScaleFactor.h>

#include <sql/Statements/BoltzmannParticleEvolution.h>
#include <sql/Filters/BoltzmannParticleEvolution.h>
#include <sql/Callbacks/BoltzmannParticleEvolution.h>

#include <sql/Statements/TempDecay.h>

#include <sql/Statements/TempOsc.h>

#include <cmd/SolveBoltzmann/StepBuilder/ParticleData.h>

class BoltzmannStepBuilderCommand : public ICommand {
private:
    /* 
        Data required for ODEINT 
    */
    typedef boost::numeric::ublas::vector<long double> state_type;
    state_type x_;
    state_type dxdt_;
    typedef boost::numeric::ublas::matrix< double > matrix_type;
    matrix_type jac_;
    double t_;

    /* 
        Data for configuration 
    */
    DbManager db_;
    Connection& connection_;
    std::shared_ptr< DataRelay > fortranInterface_;
    std::chrono::time_point<std::chrono::system_clock> startTime_;

    /* 
        Data required for constructing Boltzmann equations 
    */
    const Models::ScaleFactorPoint& reheatPoint_;
    Models::ScaleFactorPoint currentPoint_;
    std::deque<Models::Particle, boost::pool_allocator<Models::Particle>>& particles_;
    std::map< std::string, std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > >& partialWidths_;
    std::map< std::string, Models::TotalWidth >& totalWidths_;
    const std::deque< Models::ParticleEvolution >& initialParticleEvolutions_;
    bool tempDependentMassEnabled_;
    std::vector< ParticleData, boost::pool_allocator<ParticleData> > currentParticleData_;

    /* 
        Data required for handling separate steps correctly 
    */
    int ordinal_;
    bool isInitialized_;
    bool forcePost_;

    /* 
        Final data to get posted to DB 
    */
    BoltzSqlData sqlDataToPost_;
    std::deque< std::shared_ptr< Statements::IStatement >, boost::pool_allocator< std::shared_ptr< Statements::IStatement > > > evoStatements_; 
    std::deque< std::shared_ptr< Statements::IStatement >, boost::pool_allocator< std::shared_ptr< Statements::IStatement > > > particleStatements_;
    std::deque< std::shared_ptr< Statements::IStatement >, boost::pool_allocator< std::shared_ptr< Statements::IStatement > > > scaleFactorStatements_; 
    std::deque< std::shared_ptr< Statements::IStatement >, boost::pool_allocator< std::shared_ptr< Statements::IStatement > > > updateScaleFactorStatements_; 
    std::deque< std::shared_ptr< Statements::IStatement >, boost::pool_allocator< std::shared_ptr< Statements::IStatement > > > tempOscStatements_; 
    std::deque< std::shared_ptr< Statements::IStatement >, boost::pool_allocator< std::shared_ptr< Statements::IStatement > > > tempDecayStatements_; 
    std::deque< std::shared_ptr< Statements::IStatement >, boost::pool_allocator< std::shared_ptr< Statements::IStatement > > > crossSectionStatements_; 

    /* 
        Methods used for initialization 
    */
    void resetParticleData();
    ParticleData pullParticleEvolution( std::string particleKey, ParticleProductionMechanism productionMechanism, boost::uuids::uuid scaleFactorId );
    Models::ScaleFactorPoint pullPreviousScaleFactorPoint( int ordinal );

    /* 
        Methods used to compute RHS of Boltzmann equations 
    */
    double tempRadiation(long double entropy, long double scaleFactor);
    Models::ScaleFactorPoint setCurrentPoint();
    void calculateHubble();
    void setEvolutionInitialState();
    void setRadiationInitialState( ParticleData& radiation );
    void setMatterInitialState( ParticleData& matter );
    void calculateCrossSection( ParticleData& particle );
    void handleTemperatureDependence( ParticleData& particle );
    void handleTemperatureDependences();
    void updateParticleMassAndId( ParticleData& particle, std::map<string, boost::uuids::uuid>& resetKeys, int index, const ParticleData& previousParticle, const Models::ScaleFactorPoint& previousPoint );
    void addComponents();
    void checkTransitions();
    void checkOscillationTransition( ParticleData& particle );
    void checkDecayTransition( ParticleData& particle );

    /* 
        Methods used for posting data 
    */
    void postCrossSection( const ParticleData& particle );
    void cleanScaleFactorData();
    void cleanParticleData();
public:
    BoltzmannStepBuilderCommand(
        Connection& connection, 
        std::shared_ptr< DataRelay > fortranInterface, 
        const Models::ScaleFactorPoint& reheatPoint, 
        const std::deque< Models::ParticleEvolution >& initialParticleEvolutions, 
        std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles, 
        std::map< std::string, std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > >& partialWidths, 
        std::map< std::string, Models::TotalWidth >& totalWidths
    );
    ~BoltzmannStepBuilderCommand();
    void Execute() override;
    bool Exit();
    void Reset();
    bool IsInitialized();
    int getCurrentOrdinal();
    void resetCurrentOrdinal(int ordinal);
    void UpdateData( const state_type& x, const double& t );
    void SetResult();
    void Post();
    bool ForcePost();
    void UpdateScaleFactor();

    boost::numeric::ublas::vector<long double> getDxDt();
    boost::numeric::ublas::matrix<long double> getJacobian();
    boost::numeric::ublas::vector<double> UpdateInitialConditions();
};

#endif
