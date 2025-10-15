#include <io/Input/Params/ScriptRunnerParser.h>

using namespace std;

ScriptRunnerParser::ScriptRunnerParser(Logger& logger) : 
    logger_(logger)
{
    
}
ScriptRunnerParser::ScriptRunnerParser(ModelBase defaultInputs, Logger& logger) : 
    FileParser(),
    logger_(logger)
{
    __defaultParams = defaultInputs;
}
 
ScriptRunnerParser::~ScriptRunnerParser(){}

void ScriptRunnerParser::Parse(string script){
    try{
        logger_.Info("Parsing script: " + script);

        FileIO::ValidateFilePath(script, logger_);
        ifstream file;
        file.open(script);
        string line;
        vector<string> parsedContents;
        vector<ModelBase> newParams { __defaultParams };
        Overrides paramOverrides;

        while(getline(file, line)){
            string cmd = FileIO::Split(line, " ")[0];
            // don't process line if commented or blank
            if (cmd.front() != '#' && cmd.front() != '\0'){
                cmdHandler(cmd, line, newParams, paramOverrides);
            }
        }

    }
    catch (FileNotFoundException& e){
        logger_.Error( "Could not find " + script );
        logger_.Warn("Continuing using the default params...");
    }
}

void ScriptRunnerParser::cmdHandler(string cmd, string scriptLine, vector<ModelBase>& newParams, Overrides& paramOverrides){
    if (cmd == "set"){
        handleParameterOverride(scriptLine, paramOverrides);
    } else if (cmd == "scan"){
        handleParameterScan(scriptLine, paramOverrides);
    } else if (cmd == "launch"){
        auto scans = paramOverrides.ScanOverride;
        if (scans.size() > 0.){
            for (auto& scan : scans){
                scan.insert(scan.begin(), paramOverrides.SetOverride.begin(), paramOverrides.SetOverride.end());
            }
            // add the updated parameters to the list of overrides to run
            for (auto& p : scans){
                ParameterOverrides.push_back(p);
            }
        } else {
            ParameterOverrides.push_back(paramOverrides.SetOverride);
        }
        newParams = { __defaultParams };
        paramOverrides = Overrides();
    } else{
        throw logic_error("I'm not sure what you mean by \"" + cmd + "\"");
    }
}

vector<vector<string>> ScriptRunnerParser::getDirectives( string script, int nFiles ){
    ifstream fileIn;
    fileIn.open(script);

    string line;
    vector<vector<string>> directives;
    vector<string> directive;

    bool scanProcess = false;
    while(getline(fileIn, line)){
        string cmd = FileIO::Split(line, " ")[0];
        // don't process line if commented or blank
        if (cmd.front() != '#' && cmd.front() != '\0'){
            directive.push_back(line);
            if (cmd == "scan"){ scanProcess = true; }
            // if launched, we're at the end of the directive
            // copy the launch line, and see if directive needs to be split
            if(cmd == "launch"){
                // can process immediately if not a scan directive
                // otherwise need to divide up across requested cores/nodes
                if (!scanProcess){
                    directives.push_back(directive);
                } else{
                    // know nFiles, so split directive across them
                    for (int i = 0; i < nFiles; ++i){
                        vector<string> splitDirective;
                        for (auto& line : directive){
                            if (FileIO::Split(line, " ")[0] != "scan"){
                                splitDirective.push_back(line);
                            } else{
                                auto a = FileIO::Split(line, ",");
                                string reassembled;
                                for (int j = 0; j < a.size() - 1; ++j){
                                    reassembled += a[j] + ",";
                                }
                                // now process steps
                                int nPointsOld = stoi(a[a.size()-1]);
                                int nPointsNew = nPointsOld / nFiles;
                                if (i == nFiles - 1){
                                    nPointsNew += nPointsOld % nFiles;
                                }
                                reassembled += " " + to_string(nPointsNew) + "]";
                                splitDirective.push_back(reassembled);
                            }
                        }
                        directives.push_back(splitDirective);
                    }
                }
                // now reset 
                directive.clear();
                scanProcess = false;
            }
        }
    }
    return directives;
}

void ScriptRunnerParser::DivideScriptRuns(string scriptInput, ClusterRunParams runParams, string outputDirectory, string namePrefix){
    try{
        FileIO::ValidateFilePath(scriptInput, logger_);
        FileIO::CreateDirectory(outputDirectory, logger_);
                
        int nFiles = stoi(runParams.Nodes)*stoi(runParams.TaskPerNode);
        ofstream fileOuts[nFiles];
        
        auto directives = getDirectives(scriptInput, nFiles);

        for (int i=0; i<nFiles; ++i){
            fileOuts[i].open(outputDirectory + namePrefix + to_string(i) + ".dat");
            if (i < directives.size()){
                for (auto& line : directives[i]){
                    fileOuts[i] << line << "\n";
                }
            }
            fileOuts[i].close();
        }
    }
    catch(FileNotFoundException& e){
        logger_.Error( "Could not find " + string(scriptInput) );
        throw;
    }
}

vector<string> ScriptRunnerParser::scanKeys(string line){
    string subject = FileIO::Split(line, "=")[0];
    vector<string> splitSubject = FileIO::Split(subject, " &");
    vector<string> keys;
    for (auto& key : splitSubject){
        boost::to_lower(key);
        if (key != "scan" && key != ""){
            keys.push_back(key);
        }
    }
    return keys;
}

vector<string> ScriptRunnerParser::scanPredicate(string line){
    string fullPredicate = FileIO::Split(line, "=")[1];
    boost::erase_all(fullPredicate, " ");
    vector<string> splitPredicate = FileIO::Split(fullPredicate, "[]()");
    vector<string> predicates;
    for (auto& pred : splitPredicate){
        boost::to_lower(pred);
        if (pred.empty()){
            continue;
        }
        // remove any leading commas - makes sure to split correctly
        if (pred.front() == ','){
            pred.erase(0,1);
        }
        if (pred != ""){
            predicates.push_back(pred);
        }
    }

    return predicates;
}

void ScriptRunnerParser::handleParameterScan(string line, Overrides& paramOverrides){
    try{
        ScanDetails endpoints = setScanDetails(line);
        vector<vector<string>> outerParamOverrides;

        for (int i=0; i<endpoints.Steps; ++i){
            vector<string> innerParamOverrides;
            for (auto& endpoint : endpoints.ScanParams){
                std::stringstream ss;
                ss << endpoint.ScanValues[i];
                std::string newValue = ss.str();
                string scanLine = endpoint.ScanKey + "=" + newValue;
                logger_.Info("Key override: " + scanLine);
                innerParamOverrides.push_back(scanLine);
            }
            paramOverrides.ScanOverride.push_back(innerParamOverrides);
            innerParamOverrides.clear();
        }
    }
    catch (exception& e){
        logger_.Error("Could not understand scan syntax: " + line);
        logger_.Info("Use syntax 'scan <variable1> <variable2> ... <variableN>=[ (start1,end1),(start2,end2),...,(startN,endN),nPoints]'");
        throw;
    }
}

void ScriptRunnerParser::handleParameterOverride(string line, Overrides& paramOverrides){
    auto splitLine = FileIO::Split(line, " ");
    splitLine.erase(splitLine.begin());
    auto parsedLine = boost::algorithm::join(splitLine, "");
    paramOverrides.SetOverride.push_back(parsedLine);

    logger_.Info("Override: " + line);
}

ScanDetails ScriptRunnerParser::setScanDetails(string line){
    ScanDetails allEndpoints;
    vector<ScanParamDetails> scanPoints;
    try{
        auto keys = scanKeys(line);
        auto predicate = scanPredicate(line);
        // TODO: enforce the syntax better - assume that for N parameters to be scanned, number of scan points is the N+1 element
        allEndpoints.Steps = atoi(predicate[keys.size()].c_str());

        for (int i = 0; i < keys.size(); ++i){
            ScanParamDetails param;
            string scanBounds = predicate[i];
            auto parsedScanBounds = FileIO::Split(scanBounds,",;:");
            if (parsedScanBounds.size() == 1){
                param = handleImplicitBounds(parsedScanBounds, keys[i], scanPoints);
            } else{
                param = handleExplicitBounds(parsedScanBounds, keys[i], allEndpoints.Steps);
            } 
            scanPoints.push_back(param);
        }
        allEndpoints.ScanParams = scanPoints;
        return allEndpoints;
    }
    catch(exception& e){
        logger_.Error("Could not parse scan endpoints: " + line);
        throw;
    }
}

ScanParamDetails ScriptRunnerParser::handleImplicitBounds(vector<string> parsedScanBounds, string key, vector<ScanParamDetails> otherParams){
    ScanParamDetails scanParamDetails;
    // For now, just assume we're trying to match a key
    string keyToFind = parsedScanBounds[0];
    for(auto p : otherParams){
        if (p.ScanKey == keyToFind){
            scanParamDetails = p;
            break;
        }
    }
    scanParamDetails.ScanKey = key;
    return scanParamDetails;
}

ScanParamDetails ScriptRunnerParser::handleExplicitBounds(vector<string> parsedScanBounds, string key, int steps){
    ScanParamDetails scanParamDetails;
    double scanStart = atof(parsedScanBounds[0].c_str());
    double scanEnd = atof(parsedScanBounds[1].c_str());

    for (int j = 2; j<parsedScanBounds.size(); ++j){
        string scanArg = parsedScanBounds[j];
        auto drawArgs = FileIO::Split(scanArg, "~");
        if (drawArgs[0] == "rand" || drawArgs[0] == "r" || drawArgs[0] == "random"){
            scanParamDetails.Random = true;
            if (drawArgs.size() > 1){ scanParamDetails.DrawWeight = atoi( drawArgs[1].c_str() ); }
        }
        if (scanArg == "log" || scanArg == "l" || scanArg == "logarithm" || scanArg == "logarithmic") {
            scanParamDetails.LogMode = true;
        }
        if (scanArg == "startexclusive" || scanArg == "startexcl" || scanArg == "startexc"){
            scanParamDetails.StartInclusive = false;
        }
        if (scanArg == "endexclusive" || scanArg == "endexcl" || scanArg == "endexc"){
            scanParamDetails.EndInclusive = false;
        }
    }

    scanParamDetails.ScanKey = key;

    // now that the data is read from the scan, set the vector 
    if (scanParamDetails.Random){
        scanParamDetails.ScanValues = randomScanPoints(scanStart, scanEnd, steps, scanParamDetails.LogMode, scanParamDetails.StartInclusive, scanParamDetails.EndInclusive, scanParamDetails.DrawWeight);
    } else{
        scanParamDetails.ScanValues = nonrandomScanPoints(scanStart, scanEnd, steps, scanParamDetails.LogMode, scanParamDetails.StartInclusive, scanParamDetails.EndInclusive);
    }
    return scanParamDetails;
}

vector<double> ScriptRunnerParser::randomScanPoints(double start, double end, int steps, bool logMode, bool startInclusive, bool endInclusive, int randomDrawWeight = 0){
    vector<double> scanPoints;
    // TODO: implement inclusive start/end logic
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    if (logMode){
        std::uniform_real_distribution<> distr(log10(start), log10(end)); // define the range
        for (int i = 0; i < steps; ++i){
            double randVal = distr(gen);
            // if non-uniform, weight random number to degree of draw
            if (randomDrawWeight != 0){
                double randVal2 = distr(gen);
                double a = ( randVal2 - log10(start) ) / ( log10(end) - log10(start) );
                double b = ( randVal - log10(start) ) / ( log10(end) - log10(start) );
                while ( a > pow( b, randomDrawWeight ) ){
                    randVal = distr(gen);
                    randVal2 = distr(gen);
                    a = ( randVal2 - log10(start) ) / ( log10(end) - log10(start) );
                    b = ( randVal - log10(start) ) / ( log10(end) - log10(start) );
                }
            }
            scanPoints.push_back( pow(10., randVal) );
        }
    } else{
        std::uniform_real_distribution<> distr(start, end); // define the range
        for (int i = 0; i < steps; ++i){
            double randVal = distr(gen);
            // if non-uniform, weight random number to degree of draw
            if (randomDrawWeight != 0){
                double randVal2 = distr(gen);
                double a = ( randVal2 - start ) / ( end - start );
                double b = ( randVal - start ) / ( end - start );
                while ( a > pow( b, randomDrawWeight ) ){
                    randVal = distr(gen);
                    randVal2 = distr(gen);
                    a = ( randVal2 - start ) / ( end - start );
                    b = ( randVal - start ) / ( end - start );
                }
            }
            scanPoints.push_back(randVal);
        }
    }

    return scanPoints;
}

vector<double> ScriptRunnerParser::nonrandomScanPoints(double start, double end, int steps, bool logMode, bool startInclusive, bool endInclusive){
    vector<double> scanPoints;
    // TODO: implement inclusive start/end logic
    scanPoints.push_back(start);
    double val = start;
    if (abs(steps) > 1){
        while (val < end){
            double increment = 0;
            if (logMode){
                increment = abs( ( log10(end) - log10(start)) / ( steps - 1 ) );
                val = pow(10., log10(val) + increment );
            } else {
                increment = abs( (end - start) / ( steps - 1 ) );
                val = val + increment;
            }
            scanPoints.push_back(val);
        }
    } else{
        scanPoints.push_back(end);
    }

    return scanPoints;
}
