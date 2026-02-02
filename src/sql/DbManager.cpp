#include <sql/DbManager.h>

using namespace std;

DbManager::DbManager(string connectionString, Logger& logger) :
    logger_(logger)
{
    connectionString_ = connectionString;
    Open();
}

DbManager::DbManager(Connection& connection) :
    logger_(connection.Log)
{
    connectionString_ = connection.SqlConnectionString;
    Open();
}

DbManager::~DbManager()
{
    Close();
}

void DbManager::Open(){
    logger_.Debug( "Opening db: " + connectionString_ );

    auto result = sqlite3_open(connectionString_.c_str(), &db_);
    sqlite3_busy_timeout(db_, 5000); // 5 seconds
    if ( result != SQLITE_OK ){
        logger_.Error("SQL error " + to_string(result));
        sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
        throw_with_trace( exception() );
    }
}

void DbManager::Close(){
    logger_.Debug( "Closing db: " + connectionString_ );
    sqlite3_close(db_);
}

void DbManager::executeStatementPtr( 
    std::shared_ptr< Statements::IStatement > statement 
){
    string statementStr = statement -> Statement();
    if (!statementStr.empty()){
        logger_.Debug("Executing SQL statement: '" + statementStr + "'");

        char *zErrMsg = 0;
        auto res1 = sqlite3_exec(db_, "BEGIN IMMEDIATE;", NULL, NULL, NULL);
        if (res1 != SQLITE_OK) {
            throw_with_trace(exception());
        }
        auto result = sqlite3_exec(db_, statementStr.c_str(), NULL, NULL, &zErrMsg);

        if( result != SQLITE_OK ){
            std::string err;
            if (zErrMsg != nullptr){
                err = string(zErrMsg);
            }
            logger_.Error("SQL error " + to_string(result) + ": " + err);
            sqlite3_free(zErrMsg);
            sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
            throw_with_trace( exception() );
        }
        sqlite3_exec(db_, "COMMIT;", NULL, NULL, NULL);
    }
}

void DbManager::executeStatement( Statements::IStatement& statement ){
    string statementStr = statement.Statement();
    if (!statementStr.empty()){
        logger_.Debug("Executing SQL statement: '" + statementStr + "'");

        char *zErrMsg = 0;
        auto res1 = sqlite3_exec(db_, "BEGIN IMMEDIATE;", NULL, NULL, NULL);
        if (res1 != SQLITE_OK) {
            throw_with_trace(exception());
        }
        auto result = sqlite3_exec(db_, statementStr.c_str(), NULL, NULL, &zErrMsg);

        if( result != SQLITE_OK ){
            std::string err;
            if (zErrMsg != nullptr){
                err = string(zErrMsg);
            }
            logger_.Error("SQL error " + to_string(result) + ": " + err);
            sqlite3_free(zErrMsg);
            sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
            throw_with_trace( exception() );
        }
        sqlite3_exec(db_, "COMMIT;", NULL, NULL, NULL);
    }
}

void DbManager::executeStatementCallback( 
    Statements::IStatement& statement, 
    int (*callback)(void*,int,char**,char**), 
    ICallbackReturn& callbackReturn 
){
    string statementStr = statement.Statement();
    if (!statementStr.empty()){
        logger_.Debug("Executing SQL statement: '" + statementStr + "'");

        char *zErrMsg = 0;
        auto res1 = sqlite3_exec(db_, "BEGIN IMMEDIATE;", NULL, NULL, NULL);
        if (res1 != SQLITE_OK) {
            throw_with_trace(exception());
        }
        auto result = sqlite3_exec(db_, statementStr.c_str(), callback, &callbackReturn, &zErrMsg);

        if( result != SQLITE_OK ){
            std::string err;
            if (zErrMsg != nullptr){
                err = string(zErrMsg);
            }
            logger_.Error("SQL error " + to_string(result) + ": " + err);
            sqlite3_free(zErrMsg);
            sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
            throw_with_trace( exception() );
        }
        sqlite3_exec(db_, "COMMIT;", NULL, NULL, NULL);
    }
}

void DbManager::executeStatementDeque( 
    std::deque< 
        std::shared_ptr< Statements::IStatement >, 
        boost::pool_allocator< 
            std::shared_ptr< Statements::IStatement > 
        > 
    > statement 
){
    string statementStr;
    for (int i = 0; i < statement.size(); ++i){
        string st = statement[i] -> Statement();
        if (i == 0){
            statementStr = st;
        } else{
            std::vector<string> result;
            result.reserve(2);
            boost::algorithm::split_regex( result, st, boost::regex( "VALUES" ) ) ;
            statementStr.append(", ");
            statementStr.append( result[1] );
        }
    }
    statementStr.append(";");
    if (!statementStr.empty()){
        logger_.Debug("Executing SQL statement: '" + statementStr + "'");

        char *zErrMsg = 0;
        auto res1 = sqlite3_exec(db_, "BEGIN IMMEDIATE;", NULL, NULL, NULL);
        if (res1 != SQLITE_OK) {
            throw_with_trace(exception());
        }
        auto result = sqlite3_exec(db_, statementStr.c_str(), NULL, NULL, &zErrMsg);
    
        if( result != SQLITE_OK ){
            std::string err;
            if (zErrMsg != nullptr){
                err = string(zErrMsg);
            }
            logger_.Error("SQL error " + to_string(result) + ": " + err + "\n" + statementStr);
            sqlite3_free(zErrMsg);
            sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
            throw_with_trace( exception() );
        }
        sqlite3_exec(db_, "COMMIT;", NULL, NULL, NULL);
    }
}

void DbManager::Execute( 
    std::deque< 
        std::shared_ptr< Statements::IStatement >, 
        boost::pool_allocator< 
            std::shared_ptr< Statements::IStatement > 
        > 
    > statement 
){
    executeStatementDeque( statement );
}

void DbManager::Execute( 
    Statements::IStatement& statement 
){
    executeStatement( statement );
}

void DbManager::Execute( 
    Statements::IStatement& statement, 
    int (*callback)(void*,int,char**,char**), 
    ICallbackReturn& callbackReturn 
){
    executeStatementCallback( statement, callback, callbackReturn );
}


void DbManager::Configure(){
    string createTables;
    createTables += create_Input_table();
    createTables += create_Particle_table();
    createTables += create_ParticleEvolution_table();
    createTables += create_PartialWidth_table();
    createTables += create_TotalWidth_table();
    createTables += create_SigmaV_table();
    createTables += create_ScaleFactor_table();
    createTables += create_tempDecay_table();
    createTables += create_tempEquality_table();
    createTables += create_tempOsc_table();
    createTables += create_DeltaNeff_table();
    createTables += create_RelicDensity_table();
    createTables += create_CheckBBN_table();

    /* Execute SQL statement */
    char *zErrMsg = 0;
    auto result = sqlite3_exec(db_, createTables.c_str(), NULL, NULL, &zErrMsg);

    if( result != SQLITE_OK ){
        std::string err;
        if (zErrMsg != nullptr){
            err = string(zErrMsg);
        }
        logger_.Error("SQL error " + to_string(result) + ": " + err);
        sqlite3_free(zErrMsg);
        sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
        throw_with_trace( exception() );
    }
    sqlite3_exec(db_, "PRAGMA synchronous = OFF", NULL, NULL, &zErrMsg );
    sqlite3_exec(db_, "PRAGMA journal_mode = WAL", NULL, NULL, &zErrMsg );
}

string DbManager::create_Input_table(){
    string cmd = "CREATE TABLE Inputs("  \
    "ID             TEXT    PRIMARY KEY NOT NULL," \
    "ModelParams    TEXT," \
    "CosmologyParams TEXT);";
    cmd.append("CREATE INDEX input_id_asc ON Inputs(ID asc);");
    return cmd;
}

string DbManager::create_Particle_table(){
    string cmd = "CREATE TABLE Particle("  \
    "PK INTEGER PRIMARY KEY AUTOINCREMENT," \
    "ID             TEXT    NOT NULL," \
    "InputID        TEXT    NOT NULL," \
    "ParticleKey    TEXT    NOT NULL," \
    "Name           TEXT    NOT NULL," \
    "Pdg            INT     NOT NULL," \
    "Mass           REAL    NOT NULL," \
    "Spin           REAL    NOT NULL," \
    "Charges        TEXT    NOT NULL," \
    "ScaleFactorID  TEXT );";
    cmd.append("CREATE INDEX particle_id_asc ON Particle(ID asc);");
    cmd.append("CREATE INDEX particle_keyPair_asc ON Particle(ParticleKey asc, InputID asc);");
    cmd.append("CREATE INDEX particle_inputid_asc ON Particle(InputID asc);");
    return cmd;
}

string DbManager::create_ParticleEvolution_table(){
    string cmd = "CREATE TABLE ParticleEvolution("  \
    "PK INTEGER PRIMARY KEY AUTOINCREMENT," \
    "ID             TEXT    NOT NULL," \
    "InputID        TEXT    NOT NULL," \
    "ParticleKey    TEXT    NOT NULL," \
    "ParticleID     TEXT    NOT NULL," \
    "Production     INT     NOT NULL," \
    "Active         INT     NOT NULL," \
    "Y1             REAL," \
    "Y2             REAL," \
    "NumberDensity  REAL," \
    "EqNumberDensity  REAL," \
    "EnergyDensity  REAL," \
    "ScaleFactorID  TEXT    NOT NULL," \
    "IsOsc          INT," \
    "IsDecay        INT);";
    cmd.append("CREATE INDEX particleevolution_boltz_asc ON ParticleEvolution(ScaleFactorID asc, ParticleKey asc, Production asc);");
    cmd.append("CREATE INDEX particleevolution_mult_asc ON ParticleEvolution(ParticleID asc, InputID);");
    cmd.append("CREATE INDEX particleevolution_mult2_asc ON ParticleEvolution(ParticleKey asc, Production asc, InputID);");
    return cmd;
}

string DbManager::create_PartialWidth_table(){
    string cmd = "CREATE TABLE PartialWidth("  \
    "ID             TEXT    NOT NULL," \
    "InputID        TEXT    NOT NULL," \
    "TotalWidthID   TEXT    NOT NULL," \
    "ParentID       TEXT    NOT NULL," \
    "ChildrenIDs    TEXT    NOT NULL," \
    "PartialWidth   REAL    NOT NULL," \
    "ScaleFactorID  TEXT );";
    cmd.append("CREATE INDEX partialwidth_id_asc ON PartialWidth(ID asc);");
    cmd.append("CREATE INDEX partialwidth_mult_asc ON PartialWidth(ParentID asc, InputID);");
    cmd.append("CREATE INDEX partialwidth_mult2_asc ON PartialWidth(ParentID asc, InputID, ScaleFactorID);");
    return cmd;
}

string DbManager::create_TotalWidth_table(){
    string cmd = "CREATE TABLE TotalWidth("  \
    "PK INTEGER PRIMARY KEY AUTOINCREMENT," \
    "ID             TEXT    NOT NULL," \
    "InputID        TEXT    NOT NULL," \
    "PartialWidthIDs TEXT," \
    "ParentID       TEXT    NOT NULL," \
    "TotalWidth     REAL    NOT NULL," \
    "ScaleFactorID  TEXT );";
    cmd.append("CREATE INDEX totalwidth_id_asc ON TotalWidth(ID asc);");
    cmd.append("CREATE INDEX totalwidth_mult_asc ON TotalWidth(ParentID asc, InputID);");
    cmd.append("CREATE INDEX totalwidth_inputid_asc ON TotalWidth(InputID asc);");
    return cmd;
}

string DbManager::create_SigmaV_table(){
    string cmd = "CREATE TABLE SigmaV("  \
    "PK INTEGER PRIMARY KEY AUTOINCREMENT," \
    "ID             TEXT    NOT NULL," \
    "InputID        TEXT    NOT NULL," \
    "ParticleID     TEXT    NOT NULL," \
    "SigmaV         REAL," \
    "Temperature    REAL," \
    "ScaleFactorID  TEXT );";
    cmd.append("CREATE INDEX sigmav_id_asc ON SigmaV(ID asc);");
    cmd.append("CREATE INDEX sigmav_mult_asc2 ON SigmaV(ScaleFactorID asc, ParticleID asc, InputID asc);");
    return cmd;
}

string DbManager::create_ScaleFactor_table(){
    string cmd = "CREATE TABLE ScaleFactor("  \
    "PK INTEGER PRIMARY KEY AUTOINCREMENT," \
    "ID             TEXT    NOT NULL," \
    "InputID        TEXT    NOT NULL," \
    "Temperature    REAL," \
    "ScaleFactor    REAL," \
    "Entropy        REAL," \
    "Hubble         REAL," \
    "GStar          REAL    NOT NULL," \
    "GStarEntropic  REAL    NOT NULL," \
    "Ordinal        INT     NOT NULL);";
    cmd.append("CREATE INDEX scalefactor_id_asc ON ScaleFactor(ID asc);");
    cmd.append("CREATE INDEX scalefactor_inputid_desc ON ScaleFactor(InputID, Ordinal desc);");
    cmd.append("CREATE INDEX scalefactor_ordinal_desc ON ScaleFactor(Ordinal desc);");
    return cmd;
}

string DbManager::create_tempOsc_table(){
    string cmd = "CREATE TABLE TempOscillate("  \
    "ID             TEXT    NOT NULL," \
    "InputID        TEXT    NOT NULL," \
    "ParticleID     TEXT    NOT NULL," \
    "Temperature    REAL," \
    "GStar          REAL    NOT NULL," \
    "GStarEntropic  REAL    NOT NULL," \
    "ScaleFactorID  TEXT );";
    cmd.append("CREATE INDEX temposc_id_asc ON TempOscillate(ID ASC);");
    cmd.append("CREATE INDEX temposc_mult_asc ON TempOscillate(InputID, ParticleID);");
    return cmd;
}

string DbManager::create_tempDecay_table(){
    string cmd = "CREATE TABLE TempDecay("  \
    "ID             TEXT    NOT NULL," \
    "InputID        TEXT    NOT NULL," \
    "ParticleID     TEXT    NOT NULL," \
    "Temperature    REAL," \
    "GStar          REAL    NOT NULL," \
    "GStarEntropic  REAL    NOT NULL," \
    "ScaleFactorID  TEXT );";
    cmd.append("CREATE INDEX tempdecay_id_asc ON TempDecay(ID ASC);");
    cmd.append("CREATE INDEX tempdecay_mult_asc ON TempDecay(InputID, ParticleID);");
    return cmd;
}

string DbManager::create_tempEquality_table(){
    string cmd = "CREATE TABLE TempEquality("  \
    "ID             TEXT    NOT NULL," \
    "InputID        TEXT    NOT NULL," \
    "ParticleID     TEXT    NOT NULL," \
    "Production     TEXT    NOT NULL," \
    "Temperature    REAL," \
    "GStar          REAL    NOT NULL," \
    "GStarEntropic  REAL    NOT NULL," \
    "ScaleFactorID  TEXT );";
    cmd.append("CREATE INDEX tempequality_id_asc ON TempEquality(ID ASC);");
    cmd.append("CREATE INDEX tempequality_mult_asc ON TempEquality(InputID, ParticleID);");
    return cmd;
}

string DbManager::create_DeltaNeff_table(){
    string cmd = "CREATE TABLE DeltaNeff("  \
    "ID             TEXT    NOT NULL," \
    "InputID        TEXT    NOT NULL," \
    "ParentID       TEXT," \
    "ChildrenIDs    TEXT," \
    "ParticleID     TEXT    NOT NULL," \
    "ParticleEvolutionID    TEXT    NOT NULL," \
    "DeltaNeff      REAL    NOT NULL," \
    "ScaleFactorID  TEXT );";
    cmd.append("CREATE INDEX deltaneff_id_asc ON DeltaNeff(ID ASC);");
    cmd.append("CREATE INDEX deltaneff_mult_asc ON DeltaNeff(InputID, ParentID);");
    return cmd;
}

string DbManager::create_RelicDensity_table(){
    string cmd = "CREATE TABLE RelicDensity("  \
    "ID             TEXT    NOT NULL," \
    "InputID        TEXT    NOT NULL," \
    "ParticleID     TEXT    NOT NULL," \
    "ParticleEvolutionID    TEXT    NOT NULL," \
    "OmegaHSqr      REAL    NOT NULL," \
    "ScaleFactorID  TEXT );";
    cmd.append("CREATE INDEX relicdensity_id_asc ON RelicDensity(ID ASC);");
    cmd.append("CREATE INDEX relicdensity_mult_asc ON RelicDensity(InputID, ParticleID);");
    return cmd;
}

string DbManager::create_CheckBBN_table(){
    string cmd = "CREATE TABLE CheckBBN("  \
    "ID             TEXT    NOT NULL," \
    "InputID        TEXT    NOT NULL," \
    "ParticleID     TEXT    NOT NULL," \
    "ParticleEvolutionID    TEXT    NOT NULL," \
    "BrHadronic     REAL    NOT NULL," \
    "DensityConstraint      REAL    NOT NULL," \
    "DensityCalculated      REAL    NOT NULL," \
    "ConstraintSatisfied    INT    NOT NULL," \
    "ScaleFactorID  TEXT );";
    cmd.append("CREATE INDEX checkbbn_id_asc ON CheckBBN(ID ASC);");
    cmd.append("CREATE INDEX checkbbn_mult_asc ON CheckBBN(InputID, ParticleID);");
    return cmd;
}
