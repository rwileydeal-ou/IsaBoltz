#include <io/Input/FileParser.h>

using namespace std;

FileParser::FileParser(){}
FileParser::~FileParser(){}

double FileParser::ParseFrac(string line){
    vector<string> splitLine = FileIO::Split(line, "/");
    double num = atof(splitLine[0].c_str());
    if (splitLine.size() == 1){ 
        return num; 
    }
    double den = atof(splitLine[1].c_str());
    if (den == 0.){
        throw_with_trace( logic_error("Division by 0!") );
    }
    return (num / den);
}

int FileParser::ParseInt(string line){
    return atoi(line.c_str());
}


double FileParser::ParseDouble(string line){
    return atof(line.c_str());
}

bool FileParser::ParseBool(string line){
    vector<string> splitLine = FileIO::Split(line, "#");
    string truncLine = splitLine[0];
    std::string lowString = boost::algorithm::to_lower_copy(truncLine);
    if (lowString.find("true") != string::npos)
        return true;
    else if (lowString.find("false") != string::npos)
        return false;
    else 
        throw_with_trace( logic_error("Could not parse") );
}

void FileParser :: Parse(string infile, Logger& logger){
    FileIO::ValidateFilePath(infile, logger);
}
