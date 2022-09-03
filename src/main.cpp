#include <iostream>
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <string>
#include <thread>
#include <list>
#include <map>

#include "Utilities.h"
#include "AudioFileHandler.h"
#include "WavHeader.h"

static std::list<char> pressedKeys;
static std::map<char, WavHeader> keyValueMap;
static AudioFileHandler audioFileHandler;    

/* The keyboard hook. It's low level so we can't get the last key state */
/* So we just need to keep track of what keys were pressed and check them before we handle the press */

LRESULT CALLBACK KBDHOOK(int nCode, WPARAM wParam, LPARAM lParam) {

    KBDLLHOOKSTRUCT* s = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

    switch(wParam) {
        case WM_KEYDOWN: {
            char c = MapVirtualKey(s->vkCode, MAPVK_VK_TO_CHAR);
            
            if(!ListContains(pressedKeys, c)) {
                pressedKeys.push_back(c);
                
                std::map<char, WavHeader>::iterator it = keyValueMap.find(c);
                if(it != keyValueMap.end()) {
                    std::thread th(&AudioFileHandler::PlayBlock, audioFileHandler, (LPSTR)it->second.data, it->second.data_bytes, it->second.wfx, 1);
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

    //audioFileHandler.GetWaveDevicesInfo();

    // audioFileHandler.PlayWavFile("gawd.wav", 1);

    // audioFileHandler.PlayRawFile("E:\\Random\\Projects\\C++\\Homofold\\E\\omg.raw", audioFileHandler.GetDefaultWfx(), 1);

    WavHeader wavHeader;

    audioFileHandler.LoadWavFile("discord.wav", wavHeader);

    keyValueMap.insert(std::pair<char, WavHeader>('Q', wavHeader));

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