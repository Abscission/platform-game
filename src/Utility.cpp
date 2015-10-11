
//Copyright (c) 2015 Jason Light
//License: MIT

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

Win32FileContents ReadEntireFile(const char * Filename){
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

std::string Trim(std::string str) {
	size_t End = str.find_last_not_of(" \t");
	if (std::string::npos != End){
		str = str.substr(0, End + 1);
	}

	size_t Start = str.find_first_not_of(" \t");
	if (std::string::npos != Start){
		str = str.substr(0, Start);
	}

	return str;
}



u32 rgba(u8 r, u8 g, u8 b, u8 a) {
	float alpha = a / 255.f;
	return a << 24 | (int)(r * alpha) << 16 | (int)(g * alpha) << 8 | (int)(b * alpha);
}

const InstructionSet::InstructionSet_Internal InstructionSet::CPU_Rep;

hsv_color RGBtoHSV(rgba_color rgb) {
	float min, max, delta;
	min = MIN3(rgb.r, rgb.g, rgb.b);
	max = MAX3(rgb.r, rgb.g, rgb.b);

	hsv_color ret;
	ret.a = rgb.a;

	ret.v = max;				// v
	delta = max - min;
	if (max != 0)
		ret.s = delta / max;		// s
	else {
		// r = g = b = 0		// s = 0, v is undefined
		ret.s = 0;
		ret.h = -1;
		return ret;
	}
	if (rgb.r == max)
		ret.h = (rgb.g - rgb.b) / delta;		// between yellow & magenta
	else if (rgb.g == max)
		ret.h = 2 + (rgb.b - rgb.r) / delta;	// between cyan & yellow
	else
		ret.h = 4 + (rgb.r - rgb.g) / delta;	// between magenta & cyan
	ret.h *= 60;				// degrees
	if (ret.h < 0)
		ret.h += 360;

	return ret;
}

rgba_color HSVtoRGB(hsv_color in) {

	rgba_color ret = 0;
	ret.a = in.a;

	int i;
	float f, p, q, t;
	if (in.s == 0) {
		// achromatic (grey)
		ret.r = ret.g = ret.b = in.v;
		return ret;
	}
	in.h /= 60;			// sector 0 to 5
	i = floor(in.h);
	f = in.h - i;			// factorial part of h
	p = in.v * (1 - in.s);
	q = in.v * (1 - in.s * f);
	t = in.v * (1 - in.s * (1 - f));
	switch (i) {
	case 0:
		ret.r = in.v;
		ret.g = t;
		ret.b = p;
		break;
	case 1:
		ret.r = q;
		ret.g = in.v;
		ret.b = p;
		break;
	case 2:
		ret.r = p;
		ret.g = in.v;
		ret.b = t;
		break;
	case 3:
		ret.r = p;
		ret.g = q;
		ret.b = in.v;
		break;
	case 4:
		ret.r = t;
		ret.g = p;
		ret.b = in.v;
		break;
	default:		// case 5:
		ret.r = in.v;
		ret.g = p;
		ret.b = q;
		break;
	}

	return ret;
}