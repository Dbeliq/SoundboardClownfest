#include "AudioFileHandler.h"

void AudioFileHandler::GetWaveDevicesInfo() {
    UINT numOfDevs = waveOutGetNumDevs();
    std::cout << "Number of devices: " << numOfDevs << std::endl; 

    for(int devId = 0; devId < numOfDevs; devId++) {
        WAVEOUTCAPS caps;
        MMRESULT result = waveOutGetDevCaps(devId, &caps, sizeof(WAVEOUTCAPS));
        wprintf(L"Name=%s\n", caps.szPname);
        printf("Manufacturer=%x, Product=%x, Version=%x\n\n", caps.wMid, caps.wPid, caps.vDriverVersion);
    }
}


LPSTR AudioFileHandler::LoadRawAudioBlock(const char* filePath, DWORD* blockSize) {
    
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD size = 0;
    DWORD readBytes = 0;

    void* block = NULL;

    /* Openning the file */

    if((hFile = CreateFileA(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)) 
    == INVALID_HANDLE_VALUE) {
        std::cout << "Could not open file" << std::endl;
        return NULL;
    }
    
    /* Get the file size, allocate memory and read the file */

    if((size = GetFileSize(hFile, NULL)) == INVALID_FILE_SIZE) {
        std::cout << "Could not get file size" << std::endl;
        return NULL;
    }
    if((block = HeapAlloc(GetProcessHeap(), 0, size)) == NULL) {
        std::cout << "Could not allocate memory" << std::endl;
        return NULL;
    }

    ReadFile(hFile, block, size, &readBytes, NULL);

    /* Closing the handle, setting the size and returning a pointer to our data block */

    CloseHandle(hFile);

    *blockSize = size;

    return (LPSTR)block;
}

void AudioFileHandler::WriteRawAudioBlock(HWAVEOUT hWaveOut, LPSTR block, DWORD size) {

    /* Initializing the block header with the size and pointer to memory block */

    WAVEHDR header;

    ZeroMemory(&header, sizeof(WAVEHDR));
    header.dwBufferLength = size;
    header.lpData = block;

    /* Prepare the block */

    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));

    /* Write the block to the device */

    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));

    /* Keep trying to unprepare the header until it stops playing */

    while (waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR)) == WAVERR_STILLPLAYING) 
    Sleep(100);
    

}