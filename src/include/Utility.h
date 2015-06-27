
#include <Windows.h>

typedef unsigned char BYTE;

///<summary>Displays a Win32 meessage in human readable form</summary>
///<param name="Code">The error code to process</param>
void DisplayMessage(HRESULT Code) {
	//Make a buffer for the error's text
	LPTSTR ErrorText = NULL;

	//Fill it with the error based on the error code
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER| FORMAT_MESSAGE_IGNORE_INSERTS, NULL, Code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&ErrorText, 0,	NULL);

	//If the error has been formatted, display it in a messagebox, then free the buffer
	if (ErrorText != NULL)
	{
		MessageBox(0, ErrorText, "Error", MB_OK | MB_ICONERROR);

		LocalFree(ErrorText);
		ErrorText = NULL;
	}
}

struct Win32FileContents {
	long long Size;
	unsigned char * Data;
};

Win32FileContents ReadEntireFile (const char * Filename){
	HANDLE File = CreateFileA(Filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

	LARGE_INTEGER Size;
	GetFileSizeEx(File, &Size);

	Win32FileContents FileContents = {};

	FileContents.Size = Size.QuadPart;
	FileContents.Data = (unsigned char *)VirtualAlloc(0, Size.LowPart, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	DWORD BytesRead;
	ReadFile(File, FileContents.Data, Size.LowPart, &BytesRead, 0);


	if (BytesRead != Size.LowPart) {
		DisplayMessage(GetLastError());
		//TODO: Logging, errors etc
	}

	return FileContents;
};