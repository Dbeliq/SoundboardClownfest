#pragma once

#include <string>
#include <algorithm>
#include <iterator>
#include <list>
#include <Windows.h>
#include <mmsystem.h>

#include "Path.h"
#include "WavHeader.h"

/* Helper function for finding if a list contains an element */

template <typename T>
inline bool ListContains(std::list<T> & listOfElements, const T & element)
{
    // Find the iterator if element in list
    auto it = std::find(listOfElements.begin(), listOfElements.end(), element);
    // return if iterator points to end or not. It points to end then it means element
    // does not exists in list
    return it != listOfElements.end();
};