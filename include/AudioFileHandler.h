#pragma once
#include <stdio.h>
#include <string>

#include <Windows.h>
#include <mmsystem.h>

#include "WavHeader.h"

/* A class to handle audio files. */
/* Currently it loads a .raw file block into memory and plays it*/
/* Because of this the files shouldn't be large. Might implement a block switch to make larger files actually usable*/
/* Might add automated conversion to .raw file */

class AudioFileHandler {

public:
    AudioFileHandler();
    AudioFileHandler(WAVEFORMATEX);
    
    WAVEFORMATEX GetDefaultWfx();
    void GetWaveDevicesInfo();

    void SetDefaultWfx(WAVEFORMATEX wfx);
    
    void PlayRawFile(const char* filePath, int deviceId);
    void PlayBlock(LPSTR block, DWORD blockSize, WAVEFORMATEX wfx, int deviceId);
    
    LPSTR LoadRawAudioBlock(const char* filePath, DWORD* blockSize);
    void LoadWavFile(const char* filePath, WavHeader& wavHeader);
private: 
    void WriteRawAudioBlock(HWAVEOUT hWaveOut, LPSTR block, DWORD size);
    WAVEFORMATEX defaultWfx;
};