#include <iostream>


#include <iostream>
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <string>
#include <thread>

#include "Path.h"
#include "AudioFileHandler.h"



int main() {
   
    AudioFileHandler audioFileHandler;    

    //std::thread th(&AudioFileHandler::PlayRawFile, audioFileHandler, "E:\\Random\\Projects\\C++\\Homofold\\E\\omg.raw", 1);


    audioFileHandler.PlayRawFile("E:\\Random\\Projects\\C++\\Homofold\\E\\omg.raw", 1);

   // system("pause");
    return 0;
}