#include "AudioFileHandler.h"

// TODO: see what the most used format is and replace the current values with it.
AudioFileHandler::AudioFileHandler() {
    defaultWfx.nSamplesPerSec = 48000;
    defaultWfx.wBitsPerSample = 24;
    defaultWfx.nChannels = 2;
    
    defaultWfx.cbSize = 0;
    defaultWfx.wFormatTag = WAVE_FORMAT_PCM;
    defaultWfx.nBlockAlign = (defaultWfx.wBitsPerSample >> 3) * defaultWfx.nChannels;
    defaultWfx.nAvgBytesPerSec = defaultWfx.nBlockAlign * defaultWfx.nSamplesPerSec;
}

AudioFileHandler::AudioFileHandler(WAVEFORMATEX wfx) {
    // Add validation if not lazy :)
    defaultWfx = wfx;
}

WAVEFORMATEX AudioFileHandler::GetDefaultWfx() {
    return defaultWfx;
}
void AudioFileHandler::SetDefaultWfx(WAVEFORMATEX wfx) {
    // Do some validation if not lazy :3
    defaultWfx = wfx;
}



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

// TODO: Implement a way for errors to actually work
void AudioFileHandler::LoadWavFile(const char* filePath, WavHeader& wavHeader) {
    FILE* file;
    char tempDataHolder[4];
    uint32_t wavSize;
    uint32_t fmtChunkSize;

    if ((file = fopen(filePath, "rb")) == NULL) {
        printf("Couldn't open file");
        return;
    }

    fread(tempDataHolder, 4, 1, file);
    if(tempDataHolder[0] != 'R' || tempDataHolder[1] != 'I' || tempDataHolder[2] != 'F' || tempDataHolder[3] != 'F') {
        printf("riff_header value should be \"RIFF\"");
        return;
    }
    
    fread(&wavSize, 4, 1, file);

    fread(tempDataHolder, 1, 4, file);
    if(tempDataHolder[0] != 'W' || tempDataHolder[1] != 'A' || tempDataHolder[2] != 'V' || tempDataHolder[3] != 'E') {
        printf("wav_header value should be \"WAVE\"");
        return;
    }
    
    fread(tempDataHolder, 1, 4, file);
    if(tempDataHolder[0] != 'f' || tempDataHolder[1] != 'm' || tempDataHolder[2] != 't' || tempDataHolder[3] != ' ') {
        printf("fmt_header value should be \"fmt \"");
        return;
    }

    fread(&fmtChunkSize, 4, 1, file);
    
    fread(&wavHeader.wfx.wFormatTag, 2, 1, file);

    fread(&wavHeader.wfx.nChannels, 2, 1, file);

    fread(&wavHeader.wfx.nSamplesPerSec, 4, 1, file);

    fread(&wavHeader.wfx.nAvgBytesPerSec, 4, 1, file);

    fread(&wavHeader.wfx.nBlockAlign, 2, 1, file);

    fread(&wavHeader.wfx.wBitsPerSample, 2, 1,file);

    while(1) { 
        fread(tempDataHolder, 1, 4, file);
        fread(&wavHeader.data_bytes, 4, 1, file);
        if(tempDataHolder[0] == 'd' && tempDataHolder[1] == 'a' && tempDataHolder[2] == 't' && tempDataHolder[3] == 'a') {
            break;
        }
        fseek(file, wavHeader.data_bytes, SEEK_CUR);
    }

    if((wavHeader.data = HeapAlloc(GetProcessHeap(), 0, wavHeader.data_bytes)) == NULL) {
        printf("Could not allocate memory");
        return;
    }

    if(fread(wavHeader.data, 1, wavHeader.data_bytes, file) != wavHeader.data_bytes) {
        printf("Failed to read data bytes");
        return;
    }

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

void AudioFileHandler::PlayBlock(LPSTR block, DWORD blockSize, WAVEFORMATEX wfx, int deviceId) {

    /* Initializing stuff*/

    HWAVEOUT hWaveOut;
    MMRESULT result;

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

void AudioFileHandler::PlayRawFile(const char* filePath, WAVEFORMATEX wfx, int deviceId) {

    /* Initializing stuff */

    HWAVEOUT hWaveOut;
    LPSTR block; 
    DWORD blockSize;
    MMRESULT result;

    /* Loading the raw audio file into memory */

    if ((block = LoadRawAudioBlock(filePath, &blockSize)) == NULL) {
        printf("Unable to load file\n");
        return;
    }

    /* Playing the block */

    PlayBlock(block, blockSize, wfx, deviceId);

    /* Freeing the block from memory */

    HeapFree(GetProcessHeap(), 0, block);
}

void AudioFileHandler::PlayWavFile(const char* filePath, int deviceId) {

    /* Initializing stuff */

    WavHeader wavHeader;

    /* Loading the wav file into memory and filling the wave format */

    LoadWavFile(filePath, wavHeader);

    //WAVEFORMATEX wfx = WavHeaderToWfx(wavHeader);
    
    /* Playing the block */

    PlayBlock((LPSTR)wavHeader.data, wavHeader.data_bytes, wavHeader.wfx, deviceId);

    /* Freeing the block from memory */

    HeapFree(GetProcessHeap(), 0, wavHeader.data);
}