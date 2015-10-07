
//Copyright (c) 2015 Jason Light
//License: MIT

#pragma once

#include <string>
#include <map>

#include <cstdarg>

class Log {
private:
	std::string FileName;

public:
	Log(std::string FileName);
	void Write(std::string String);
	void WriteF(std::string Format, ...);

};

extern Log GlobalLog;
