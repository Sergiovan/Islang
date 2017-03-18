//
// Created by Sergiovan on 18-Mar-17.
//
#ifdef __WIN32__
#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <shlwapi.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x04
#endif

#endif

#include <algorithm>
#include <string>

#include "Compat.h"

namespace compat {
    void init(){
#ifdef __WIN32__
        /* Garbage needed for windows console to take ANSI codes */
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleOutputCP(65001);
        SetConsoleMode(hOut, dwMode);
#endif
        return;
    }

    bool isRelativePath(std::string path){
#ifdef __WIN32__
        {
            std::string wpath = path;
            std::replace(wpath.begin(), wpath.end(), '/', '\\');
            return (bool) PathIsRelative(wpath.c_str());
        }
#else
        return !path.empty() && path[0] == '/';
#endif
    }
}