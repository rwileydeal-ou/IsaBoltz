#ifndef DbManager_h
#define DbManager_h

#include <string>
#include <deque>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <boost/algorithm/string_regex.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <sqlite3.h>
#include <io/Logging/Logger.h>
#include <Models/Connection.h>
#include <sql/IStatement.h>
#include <sql/ICallbackReturn.h>
#include <sql/Statements/BoltzmannParticleEvolution.h>

class DbManager
{
private:
    std::string connectionString_;
    sqlite3* db_;
    Logger& logger_;

    // the following are helper methods for the configure function
    std::string create_Input_table();
    std::string create_Particle_table();
    std::string create_ParticleEvolution_table();
    std::string create_PartialWidth_table();
    std::string create_TotalWidth_table();
    std::string create_SigmaV_table();
    std::string create_ScaleFactor_table();

    std::string create_tempOsc_table();
    std::string create_tempDecay_table();
    std::string create_tempEquality_table();

    std::string create_DeltaNeff_table();
    std::string create_RelicDensity_table();

    void executeStatementPtr( std::shared_ptr< Statements::IStatement > statement );
    void executeStatement( Statements::IStatement& statement );
    void executeStatementCallback( Statements::IStatement& statement, int (*callback)(void*,int,char**,char**), ICallbackReturn& callbackReturn );
    void executeStatementDeque( std::deque< std::shared_ptr< Statements::IStatement >, boost::pool_allocator< std::shared_ptr< Statements::IStatement > > > statement );
public:
    void Open();
    void Close();
    void Execute( std::deque< std::shared_ptr< Statements::IStatement >, boost::pool_allocator< std::shared_ptr< Statements::IStatement > > > statement );
    void Execute( Statements::IStatement& statement );
    void Execute( Statements::IStatement& statement, int (*callback)(void*,int,char**,char**), ICallbackReturn& callbackReturn );
    void Configure();
    DbManager(std::string connectionString, Logger& logger);
    DbManager(Connection& connection);
    ~DbManager();
};

#endif
