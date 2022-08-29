#pragma once
#include <stdio.h>
#include <string>

#include <Windows.h>
#include <mmsystem.h>

#include "WavHeader.h"
#include "Utilities.h"
/* A class to handle audio files. */
/* Currently it loads a .raw file block or a .wav file into memory and plays it*/
/* Because of this the files shouldn't be large. Might implement a block switch to make larger files actually usable*/

/* Using the default wfx is basically your last resort. Only use it for the .raw files whose format isn't known */
/* The waveOutWrite function is very weird when the wfx format is different than the data block */
/* There's a big chance that it won't work and the sound will be anywhere 
   in between from slighly faster version of the file to static that will obliterate your eardrums */
/* Ideally you would just load a .wav file and get the information you need from the header */



class AudioFileHandler {

public:
    AudioFileHandler();
    AudioFileHandler(WAVEFORMATEX);

    WAVEFORMATEX GetDefaultWfx();
    void SetDefaultWfx(WAVEFORMATEX wfx);

    void GetWaveDevicesInfo();
    
    LPSTR LoadRawAudioBlock(const char* filePath, DWORD* blockSize);
    void LoadWavFile(const char* filePath, WavHeader& wavHeader);

    void PlayBlock(LPSTR block, DWORD blockSize, WAVEFORMATEX wfx, int deviceId);
    void PlayRawFile(const char* filePath, WAVEFORMATEX wfx, int deviceId);
    void PlayWavFile(const char* filePath, int deviceId);
private: 
    void WriteRawAudioBlock(HWAVEOUT hWaveOut, LPSTR block, DWORD size);
    WAVEFORMATEX defaultWfx; 
};