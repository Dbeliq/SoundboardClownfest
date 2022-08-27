#include "Path.h"


Path::Path() {

    /* Get the executable directory */

    char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH); 
    executableDir = std::string(buffer);

    /* Get the root directory of the executable */

    std::string::size_type positionOfLastBackslash = executableDir.find_last_of("\\/");
    rootDir = executableDir.substr(0, positionOfLastBackslash + 1); // + 1 so we can add the backslash to the string
}


std::string Path::GetExecutableDirectory() {
    return executableDir;
}

std::string Path::GetExecutableRootDirectory() {
    return rootDir;
}   


