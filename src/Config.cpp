
//Copyright (c) 2015 Jason Light
//License: MIT

#include "Config.h"
#include <fstream>
#include "Types.h"
#include "Utility.h"

bool ConfigFile::Load(std::string Filename) {
	std::ifstream FileStream;
	FileStream.open(Filename, std::fstream::in);

	std::string Line, Key, Val;

	while (std::getline(FileStream, Line)) {
		if (Line[0] == '#') continue;

		u64 Delimiter = Line.find("="); // resolutionx=1920 => 11
		if (Delimiter == std::string::npos) {
			//invalid line
			continue;
		}
		else {
			Key = Line.substr(0, Delimiter);
			Val = Line.substr(Delimiter + 1, std::string::npos);

			Configs.insert(std::pair<std::string, std::string>(Key, Val));
		}
	}
	FileStream.close();

	return true;
}

bool ConfigFile::Save(std::string Filename){
	std::ofstream FileStream;
	FileStream.open(Filename, std::fstream::in | std::fstream::out | std::fstream::trunc);


	for (auto CVar : Configs) {
		FileStream << CVar.first << "=" << CVar.second << '\n'; // 
	}

	FileStream.flush();
	FileStream.close();

	return true;
}

ConfigFile::ConfigFile(std::string Filename) {
	this->Filename = Filename;
	Load(Filename);
}

std::string ConfigFile::Get(std::string Key, std::string Default) {
	std::map<std::string, std::string>::iterator it = Configs.find(Key);
	if (it != Configs.end()) {
		//the key exists
		return it->second;
	}
	else {
		Set(Key, Default);
		return Default;
	}
}

std::string ConfigFile::Get(std::string Key) {
	std::map<std::string, std::string>::iterator it = Configs.find(Key);
	if (it != Configs.end()) {
		//the key exists
		return it->second;
	}
	else {
		return "";
	}
}

bool ConfigFile::Set(std::string Key, std::string Val){
	Configs[Key] = Val;
	Save(Filename);
	return true;
}

char get_vk_code(std::string key) {
	const static std::map <std::string, char> keys{
		{ "q", 'Q' },{ "w", 'W' },{ "e", 'E' },{ "r", 'R' },{ "t", 'T' },{ "y", 'Y' },{ "u", 'U' },{ "i", 'I' },{ "o", 'O' },{ "p", 'P' },
		{ "a", 'A' },{ "s", 'S' },{ "d", 'D' },{ "f", 'F' },{ "g", 'G' },{ "h",'H' },{ "j", 'J' },{ "k", 'K' },{ "l", 'L' },
		{ "z", 'Z' },{ "x", 'X' },{ "c", 'C' },{ "v", 'V' },{ "b", 'B' },{ "n", 'N' },{ "m", 'M'},{ "comma", VK_OEM_COMMA}, { "period", VK_OEM_PERIOD},
		{ "f1", VK_F1 },{ "f2", VK_F2 },{ "f3", VK_F3 },{ "f4", VK_F4 },{ "f5", VK_F5 },{ "f6", VK_F6 },{ "f7", VK_F7 },{ "f8", VK_F8 },{"f9", VK_F9},{"f10", VK_F10},{"f11", VK_F11},{"f12", VK_F12},
		{ "space", (char)VK_SPACE },{ "shift", (char)VK_SHIFT },{ "left_shift", (char)VK_LSHIFT },{ "right_shift", (char)VK_RSHIFT },
		{ "ctrl", (char)VK_CONTROL },{ "control", (char)VK_CONTROL }, {"left_ctrl", VK_LCONTROL}, {"right_ctrl", VK_RCONTROL}, 
		{ "left_control", (char)VK_LCONTROL },{ "right_control", (char)VK_RCONTROL }
	};

	toLower((char*)key.c_str());

	auto it = keys.find(key);
	if (it != keys.end()) {
		return it->second;
	}
	else return 0;
}