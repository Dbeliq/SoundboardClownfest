#pragma once

#include <string>
#include <algorithm>
#include <iterator>
#include <mmsystem.h>

#include "Path.h"
#include "WavHeader.h"

/* Helper function for finding if a list contains an element */

template <typename T>
bool ListContains(std::list<T> & listOfElements, const T & element)
{
    // Find the iterator if element in list
    auto it = std::find(listOfElements.begin(), listOfElements.end(), element);
    // return if iterator points to end or not. It points to end then it means element
    // does not exists in list
    return it != listOfElements.end();
};

/* Helper function to transform WavHeader to WFX  */

WAVEFORMATEX WavHeaderToWfx(WavHeader wavHeader) {
    WAVEFORMATEX wfx;
    wfx.nSamplesPerSec = wavHeader.sample_rate;
    wfx.wBitsPerSample = wavHeader.bits_per_sample;
    wfx.nChannels = wavHeader.num_channels;
    
    wfx.cbSize = 0;
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nBlockAlign = wavHeader.block_align;
    wfx.nAvgBytesPerSec = wavHeader.byte_rate;

    return wfx;
}