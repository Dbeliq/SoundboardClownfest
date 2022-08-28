#include <iostream>


#include <iostream>
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <string>
#include <thread>
#include <list>
#include <algorithm>
#include <iterator>

#include "Path.h"
#include "AudioFileHandler.h"

static std::list<char> pressedKeys;
static AudioFileHandler audioFileHandler;    
static LPSTR block;
static DWORD blockSize;

/* Helper function for finding if a list contains an element */
/* Maybe add it to a utils class? */

template <typename T>
bool contains(std::list<T> & listOfElements, const T & element)
{
    // Find the iterator if element in list
    auto it = std::find(listOfElements.begin(), listOfElements.end(), element);
    // return if iterator points to end or not. It points to end then it means element
    // does not exists in list
    return it != listOfElements.end();
}

/* The keyboard hook. It's low level so we can't get the last key state */
/* So we just need to keep track of what keys were pressed and check them before we handle the press */

LRESULT CALLBACK KBDHOOK(int nCode, WPARAM wParam, LPARAM lParam) {

    KBDLLHOOKSTRUCT* s = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

    switch(wParam) {
        case WM_KEYDOWN: {
            char c = MapVirtualKey(s->vkCode, MAPVK_VK_TO_CHAR);
            
            if(!contains(pressedKeys, c)) {
                pressedKeys.push_back(c);
                if(s->vkCode == 0x41) {
                    std::thread th(&AudioFileHandler::PlayRawFile, audioFileHandler, "E:\\Random\\Projects\\C++\\Homofold\\E\\test.raw", 1);
                    th.detach();
                }
                if(c == 'S') {
                    std::thread th(&AudioFileHandler::PlayRawFile, audioFileHandler, "E:\\Random\\Projects\\C++\\Homofold\\E\\omg.raw", 1);
                    th.detach();
                }
                if(c == 'D') {
                    std::thread th(&AudioFileHandler::PlayBlock, audioFileHandler, block, blockSize, 1);
                    th.detach();
                }
            }      
            break;  
        }
        case WM_KEYUP: {
            char c = MapVirtualKey(s->vkCode, MAPVK_VK_TO_CHAR);
            pressedKeys.remove(c);
            break;
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main() {
    
    printf("Main started\n");

    audioFileHandler.GetWaveDevicesInfo();

   
    block = audioFileHandler.LoadRawAudioBlock("E:\\Random\\Projects\\C++\\Homofold\\E\\omg.raw", &blockSize);


    HHOOK kbd = SetWindowsHookEx(WH_KEYBOARD_LL, &KBDHOOK, 0, 0);

    MSG message;
    while(GetMessage(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    UnhookWindowsHookEx(kbd);

    system("pause");
    return 0;
}