#pragma once
#include <libloaderapi.h>
#include <string>

class Path {

public: 
    Path();
    std::string GetExecutableDirectory();
    std::string GetExecutableRootDirectory();
private:
    std::string executableDir;
    std::string rootDir;
};
