/*
    Copyright (C) 2022  habi

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifdef _WIN32
#include <windows.h>
#else 
#ifndef FALSE
#define FALSE               0
#endif
#ifndef TRUE
#define TRUE                1
#endif
typedef unsigned int DWORD;
typedef unsigned int BOOL;
typedef unsigned char       BYTE;
typedef float               FLOAT;
typedef unsigned short      WORD;
typedef char                CHAR;
typedef char* PCHAR;
long GetTickCount();
#endif