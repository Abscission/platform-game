
//Copyright (c) 2015 Jason Light
//License: MIT

#pragma once

#include <string>
#include <map>

class Log {
private:
	std::string FileName;

public:
	Log(std::string FileName);
	void Write(std::string String);

};
