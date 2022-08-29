#pragma once
#include <stdint.h>

typedef struct WavHeader
{
    /* RIFF Chunk */

    char riff_header[4];      // Should contain "RIFF"
    uint32_t wav_size;        // Size of the .wav file - 8 bytes. It doesn't count itself and the riff header
    char wav_header[4];          // Should contain "WAVE"

    /* Format sub-chunk */

    char fmt_header[4];       // Should contain "fmt "
    uint32_t fmt_chunk_size;  // Should be 16 for PCM
    uint16_t audio_format;    // Should be 1 for PCM
    uint16_t num_channels;    // 1 mono, 2 stereo ...
    uint32_t sample_rate;     // 41000, 48000 ...
    uint32_t byte_rate;       // sample_rate * num_channels * Bytes Per Sample
    uint16_t block_align;     // num_channels * Bytes Per Sample
    uint16_t bits_per_sample; // Number of bits per sample

    /* Data sub-chunk */

    char data_header[4];      // Should be "data"
    uint32_t data_bytes;      // Size of the data
    void* data;               // Pointer to the data

} WavHeader;
