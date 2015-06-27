
#include "Utility.h"

void DisplayMessage(HRESULT Code) {
	//Make a buffer for the error's text
	LPTSTR ErrorText = NULL;

	//Fill it with the error based on the error code
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, Code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&ErrorText, 0, NULL);

	//If the error has been formatted, display it in a messagebox, then free the buffer
	if (ErrorText != NULL)
	{
		MessageBox(0, ErrorText, "Error", MB_OK | MB_ICONERROR);

		LocalFree(ErrorText);
		ErrorText = NULL;
	}
}