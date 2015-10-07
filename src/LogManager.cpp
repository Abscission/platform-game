
//Copyright (c) 2015 Jason Light
//License: MIT
//
#include "LogManager.h"
#include <fstream>
#include <Windows.h>

Log::Log(std::string FileName){
	std::ofstream FileStream;
	FileStream.open(FileName, std::ofstream::out);
	FileStream << "This is a place holder top line for log files\n";
	FileStream.close();
	this->FileName = FileName;
}

void Log::Write(std::string String){
	std::ofstream FileStream;
	FileStream.open(FileName, std::ofstream::app);
	FileStream << String << "\n";
	FileStream.close();

	OutputDebugString(String.c_str());


}

void Log::WriteF(std::string Format, ...) {
	va_list VA;

	va_start(VA, Format);
	char buffer[1024];
	vsprintf_s(buffer, Format.c_str(), VA);
	va_end(VA);

	Write(buffer);
}

Log GlobalLog("platform-game-log.txt");