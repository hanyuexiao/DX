//
// Created by admin on 2025/5/6.
//

#ifndef DX_HEAD_H
#define DX_HEAD_H

#endif //DX_HEAD_H


#pragma once
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>
#include <tchar.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <io.h>
#include <fcntl.h>
#include <locale>
#include <codecvt>

using namespace std;
#pragma once

enum DATATYPE
{
    DATATYPE_MONSTERS,
    DATATYPE_TOOLS,
    DATATYPE_MAP,
};

#ifdef _UNICODE
#define TSTRING wstring
#define TCOUT wcout
#else
#define TString string
#define TCOUT cout
#endif

