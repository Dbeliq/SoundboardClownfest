#include <iostream>
#include <cstring>

#include <libloaderapi.h>
#include <string>

#include "Path.h"


int main() {
   
    Path path;

    std::cout << path.GetExecutableDirectory() << std::endl;
    std::cout << path.GetExecutableRootDirectory() << std::endl;
    
    return 0;
}