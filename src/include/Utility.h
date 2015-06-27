#pragma once

#include <Types.h>
#include <Windows.h>

struct Win32FileContents {
	long long Size;
	unsigned char * Data;
};

///<summary>Displays a Win32 meessage in human readable form</summary>
///<param name="Code">The error code to process</param>
void DisplayMessage(HRESULT Code);

Win32FileContents ReadEntireFile(const char * Filename);