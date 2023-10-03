#ifndef FileParser_h
#define FileParser_h

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <boost/filesystem.hpp>
#include <vector>
#include <io/FileIO.h>

class FileParser{
    public:
        static double ParseFrac(std::string string);
        static bool ParseBool(std::string string);
        static double ParseDouble(std::string string);
        static int ParseInt(std::string string);
        FileParser();
        ~FileParser();
        virtual void Parse(std::string infile, Logger& logger);
};

#endif
