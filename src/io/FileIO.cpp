#include <io/FileIO.h>

using namespace std;
void FileIO::ValidateFilePath(string filePath, Logger& logger){
    if (!PathExists(filePath, logger)){
        throw FileNotFoundException();
    }
}

bool FileIO::PathExists(string path, Logger& logger){
    logger.Debug("Checking if path exists: " + path);
    boost::filesystem::path p(path);
    return boost::filesystem::exists(p);
}

void FileIO::CreateDirectory(boost::filesystem::path path, Logger& logger){
    if (!boost::filesystem::exists(path)){
        logger.Info("Creating directory: " + path.string());
        boost::filesystem::create_directory(path);
    }
}

void FileIO::CreateDirectory(string path, Logger& logger){
    boost::filesystem::path dirPath = path;
    CreateDirectory(dirPath, logger);
}

bool FileIO::LineContains(string substring, string line, bool caseSensitive){
    if (line.length() > 0){
        vector<string> splitLine = Split(line, "#");
        string truncLine = splitLine[0];
        if (truncLine != ""){
            // make case insensitive
            if(!caseSensitive){
                boost::to_lower(substring);
                boost::to_lower(truncLine);
            }
            if (truncLine.find(substring) != string::npos){
                return true;
            }
        }
    }
    return false;
}

vector<string> FileIO::Split(const string& str, const string& delims = " ")
{
    vector<string> result;
    boost::split(result, str, boost::is_any_of(delims), boost::token_compress_on);
    return result;
}

string FileIO::RunDirectory(string basePath, string directoryPrefix, string delimiter, Logger& logger){
    FileIO::CreateDirectory(basePath, logger);
    vector<int> existingRuns;
    for(boost::filesystem::directory_iterator itr(basePath); itr!=boost::filesystem::directory_iterator(); ++itr){
        string dirName = itr->path().filename().string();
        if (dirName.find(directoryPrefix) != string::npos){
            int runNumber = atoi(Split(dirName, delimiter)[1].c_str());
            existingRuns.push_back(runNumber);
        }
    }
    int maxRun = 0;
    if (existingRuns.size() > 0){
        maxRun = *max_element(existingRuns.begin(), existingRuns.end());
        // increment by 1 so we can have next run if already exist
        maxRun += 1;
    }
    string runDirPath = basePath + "/" + directoryPrefix + delimiter + to_string(maxRun) + "/";
    logger.Debug("Running out of directory: " + runDirPath);
    return runDirPath;
}
