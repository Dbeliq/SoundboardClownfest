#include "AudioFileHandler.h"

AudioFileHandler::AudioFileHandler() {
    defaultWfx.nSamplesPerSec = 48000;
    defaultWfx.wBitsPerSample = 16;
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

void AudioFileHandler::LoadWavFile(const char* filePath, WavHeader& wavHeader) {
    FILE* file;

    if ((file = fopen(filePath, "rb")) == NULL) {
        printf("Couldn't open file");
        return;
    }

    fread(wavHeader.riff_header, 4, 1, file);
    if(wavHeader.riff_header[0] != 'R' || wavHeader.riff_header[1] != 'I' || wavHeader.riff_header[2] != 'F' || wavHeader.riff_header[3] != 'F') {
        printf("riff_header value should be \"RIFF\"");
        return;
    }
    
    fread(&wavHeader.wav_size, 4, 1, file);

    fread(wavHeader.wav_header, 1, 4, file);
    if(wavHeader.wav_header[0] != 'W' || wavHeader.wav_header[1] != 'A' || wavHeader.wav_header[2] != 'V' || wavHeader.wav_header[3] != 'E') {
        printf("wav_header value should be \"WAVE\"");
        return;
    }
    
    fread(wavHeader.fmt_header, 1, 4, file);
    if(wavHeader.fmt_header[0] != 'f' || wavHeader.fmt_header[1] != 'm' || wavHeader.fmt_header[2] != 't' || wavHeader.fmt_header[3] != ' ') {
        printf("fmt_header value should be \"fmt \"");
        return;
    }

    fread(&wavHeader.fmt_chunk_size, 4, 1, file);
    fread(&wavHeader.audio_format, 2, 1, file);
    printf("AudioFormat: %u\n", wavHeader.audio_format);
    fread(&wavHeader.num_channels, 2, 1, file);
    printf("NumChannel: %u\n", wavHeader.num_channels);
    fread(&wavHeader.sample_rate, 4, 1, file);
    printf("SampleRate: %u\n", wavHeader.sample_rate);
    fread(&wavHeader.byte_rate, 4, 1, file);
    printf("ByteRate: %u\n", wavHeader.byte_rate);
    fread(&wavHeader.block_align, 2, 1, file);
    printf("BlockAlign: %u\n", wavHeader.block_align);
    fread(&wavHeader.bits_per_sample, 2, 1,file);
    printf("BitsPerSample: %u\n", wavHeader.bits_per_sample);


    while(1) { 
        fread(wavHeader.data_header, 1, 4, file);
        printf("%c%c%c%c\n", wavHeader.data_header[0], wavHeader.data_header[1], wavHeader.data_header[2], wavHeader.data_header[3]);
        fread(&wavHeader.data_bytes, 4, 1, file);
        printf("%d\n", wavHeader.data_bytes);
        if(wavHeader.data_header[0] == 'd' && wavHeader.data_header[1] == 'a' && wavHeader.data_header[2] == 't' && wavHeader.data_header[3] == 'a') {
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

    HeapFree(GetProcessHeap(), 0, block);

    printf("Playback finished\n");
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
