#pragma once
#include <stdio.h>
#include <string>

#include <Windows.h>
#include <mmsystem.h>


/* A class to handle audio files. */
/* Currently it loads a .raw file block into memory and plays it on a separate thread */
/* Because of this the files shouldn't be large. Might implement a block switch to make larger files actually usable*/
/* Might add automated conversion to .raw file */

class AudioFileHandler {

public:
   // AudioFileHandler();
    void GetWaveDevicesInfo();
    void PlayRawFile(const char* filePath, int deviceId);
private: 
    LPSTR LoadRawAudioBlock(const char* filePath, DWORD* blockSize);
    void WriteRawAudioBlock(HWAVEOUT hWaveOut, LPSTR block, DWORD size);
};