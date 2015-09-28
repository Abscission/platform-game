
//Copyright (c) 2015 Jason Light
//License: MIT

#include "Config.h"
#include <fstream>
#include "Types.h"

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