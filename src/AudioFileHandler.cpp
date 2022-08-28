#include "AudioFileHandler.h"

void AudioFileHandler::GetWaveDevicesInfo() {
    UINT numOfDevs = waveOutGetNumDevs();
    printf("Number of devices: %u\n", numOfDevs);

    for(int devId = 0; devId < numOfDevs; devId++) {
        WAVEOUTCAPS caps;
        MMRESULT result = waveOutGetDevCaps(devId, &caps, sizeof(WAVEOUTCAPS));
        wprintf(L"Device Id: %u, Name: %S\n", devId, caps.szPname);
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
        printf("Could not open file\n");
    }
    
    /* Get the file size, allocate memory and read the file */

    if((size = GetFileSize(hFile, NULL)) == INVALID_FILE_SIZE) {
        printf("Could not get file size\n");
        return NULL;
    }
    if((block = HeapAlloc(GetProcessHeap(), 0, size)) == NULL) {
        printf("Could not allocate memory\n");
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

void AudioFileHandler::PlayRawFile(const char* filePath, int deviceId) {

    /* Initializing stuff*/

    HWAVEOUT hWaveOut;

    WAVEFORMATEX wfx;

    LPSTR block; 
    DWORD blockSize;
    MMRESULT result;


    /* The Wave Format. Would be a good idea to make it into a variable which is stored in the object */
    /* or to pass it on as a variable to the function */

    wfx.nSamplesPerSec = 48000;
    wfx.wBitsPerSample = 24;
    wfx.nChannels = 2;
    
    wfx.cbSize = 0;
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nBlockAlign = (wfx.wBitsPerSample >> 3) * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

    /* Opening the device */
    /* WAVE_MAPPER is the default device */

    if(waveOutOpen(&hWaveOut, deviceId, &wfx, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
        if(deviceId == WAVE_MAPPER) {
            printf("Default device could not be opened\n");
        }
        else {
            printf("Device %x could not be opened\n", deviceId);
        }
    }
    else {
        if(deviceId == WAVE_MAPPER) {
            printf("Default device was opened successfully!\n");
        }
        else {
            printf("Device %x was opened successfully!\n", deviceId);
        }
    }

    /* Loading the raw audio file into memory */

   if ((block = LoadRawAudioBlock(filePath, &blockSize)) == NULL) {
        printf("Unable to load file\n");
        return;
    }

    /* Writing the audio block to the device and closing the device after it finishes */

    WriteRawAudioBlock(hWaveOut, block, blockSize);

    waveOutClose(hWaveOut);

    HeapFree(GetProcessHeap(), NULL, block);

    printf("Playback finished\n");
}

void AudioFileHandler::PlayBlock(LPSTR block, DWORD blockSize, int deviceId) {

    /* Initializing stuff*/

    HWAVEOUT hWaveOut;
    WAVEFORMATEX wfx;
    MMRESULT result;

    /* The Wave Format. Would be a good idea to make it into a variable which is stored in the object */
    /* or to pass it on as a variable to the function */

    wfx.nSamplesPerSec = 48000;
    wfx.wBitsPerSample = 24;
    wfx.nChannels = 2;
    
    wfx.cbSize = 0;
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nBlockAlign = (wfx.wBitsPerSample >> 3) * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

    /* Opening the device */
    /* WAVE_MAPPER is the default device */

    if(waveOutOpen(&hWaveOut, deviceId, &wfx, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
        if(deviceId == WAVE_MAPPER) {
            printf("Default device could not be opened\n");
        }
        else {
            printf("Device %u could not be opened\n", deviceId);
        }
    }
    else {
        if(deviceId == WAVE_MAPPER) {
            printf("Default device was opened successfully!\n");
        }
        else {
            printf("Device %u was opened successfully!\n", deviceId);
        }
    }

    /* Writing the audio block to the device and closing the device after it finishes */

    WriteRawAudioBlock(hWaveOut, block, blockSize);

    waveOutClose(hWaveOut);

    printf("Playback finished\n");
}
