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