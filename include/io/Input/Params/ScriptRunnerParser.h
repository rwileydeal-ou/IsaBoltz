#ifndef ScriptRunnerParser_h
#define ScriptRunnerParser_h

#include <list>
#include <random>
#include <io/FileIO.h>
#include <io/Input/FileParser.h>
#include <io/Input/Params/ParamCardParser.h>
#include <io/Logging/Logger.h>
#include <io/Input/Run/RunParams.h>
#include <boost/algorithm/string/join.hpp>

struct ScanParamDetails{
    bool Random = false;
    bool LogMode = false;
    bool StartInclusive = true;
    bool EndInclusive = true;
    int DrawWeight = 0.;
    std::string ScanKey;
    std::vector<double> ScanValues;
};

struct ScanDetails{
    std::vector<ScanParamDetails> ScanParams;
    int Steps;
};

// Use vector of "set" to account for multiple "set" values
// Use vector of vectors of "scan" to allow for multiple points, each changing multiple values
// The "set" will then apply to ALL points in this scan
struct Overrides{
    std::vector<std::vector<std::string>> ScanOverride;
    std::vector<std::string> SetOverride;
};

class ScriptRunnerParser : public FileParser
{
    private:
        Logger& logger_;
        std::vector<std::string> scanKeys(std::string line);
        std::vector<std::string> scanPredicate(std::string line);
        std::vector<double> randomScanPoints(double start, double end, int steps, bool logMode, bool startInclusive, bool endInclusive, int randomDrawWeight);
        std::vector<double> nonrandomScanPoints(double start, double end, int steps, bool logMode, bool startInclusive, bool endInclusive);
        ScanParamDetails handleExplicitBounds(std::vector<std::string> parsedScanBounds, std::string key, int steps);
        ScanParamDetails handleImplicitBounds(std::vector<std::string> parsedScanBounds, std::string key, std::vector<ScanParamDetails> otherParams);
        ModelBase __defaultParams;
        void cmdHandler(std::string cmd, std::string scriptLine, std::vector<ModelBase>& newParams, Overrides& paramOverrides);
        void handleParameterOverride(std::string line, Overrides& paramOverrides);
        void handleParameterScan(std::string line, Overrides& paramOverrides);
        static std::vector<std::vector<std::string>> getDirectives( std::string script, int nFiles );
    public:
        ScanDetails setScanDetails(std::string scanLine);
        std::list <ModelBase> Models;
        std::list<std::vector<std::string>> ParameterOverrides;
        ScriptRunnerParser(ModelBase defaultInputs, Logger& logger);
        ScriptRunnerParser(Logger& logger);
        ~ScriptRunnerParser();
        using FileParser::Parse;
        void Parse(std::string script);
        void DivideScriptRuns(std::string scriptInput, ClusterRunParams runParams, std::string outputDirectory, std::string namePrefix);
};

#endif
