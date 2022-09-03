#pragma once
#include <stdint.h>

typedef struct WavHeader
{

    uint32_t data_bytes;      // Size of the data
    void* data;               // Pointer to the data
    WAVEFORMATEX wfx;         // The format of the data

} WavHeader;
