
//Copyright (c) 2015 Jason Light
//License: MIT

#pragma once

#include <Types.h>
#include <Windows.h>

#include <string>

struct Win32FileContents {
	long long Size;
	unsigned char * Data;
};

std::string Trim(std::string str);

///<summary>Displays a Win32 meessage in human readable form</summary>
///<param name="Code">The error code to process</param>
void DisplayMessage(HRESULT Code);

Win32FileContents ReadEntireFile(const char * Filename);

//Sum all the positive integers from 0 to N
#define SUMTO(N) (N*(N+1)/2)

//Sum all the integers between A and B
#define SUMBETWEEN(A, B) (SUMTO(B) - SUMTO(A - 1))

u32 rgba(u8 r, u8 g, u8 b, u8 a);

//Asserions
#ifdef _DEBUG
#define assertnoreason(n) if (n); else { OutputDebugStringA("Assertion Failed: "); OutputDebugStringA(#n); OutputDebugStringA("\n");  exit(455327); }
#define assert(n, d) if (n); else { OutputDebugStringA("Assertion Failed: "); OutputDebugStringA(d); OutputDebugStringA("\n");  exit(455327); }
#else
#define assertnoreason(n);
#define assert(n, d);
#endif