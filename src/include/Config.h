//Copyright (c) 2015 Jason Light
//License: MIT

#pragma once
#include <string>
#include <map>

class ConfigFile {
private:
	std::map<std::string, std::string> Configs;

	std::string Filename;

	bool Load(std::string Filename);
	bool Save(std::string Filename);

public:
	ConfigFile(std::string Filename);

	///<summary>Returns value from config file</summary>
	///<param name="Key">Identfier of config value</param>
	///<returns>Value for the config entery</returns>
	std::string Get(std::string Key);

	///<summary>Returns value from config file</summary>
	///<param name="Key">Identfier of config value</param>
	///<param name="Default">Sets the value of a config entery if it doesn't exist</parma>
	///<returns>Value for the config entery</returns>
	std::string Get(std::string Key, std::string Default);

	///<summary>Set the value of a config entery</summary>
	///<param name="Key">Identfier of config value</param>
	///<param name="Val">Value for the config entery</param>
	bool Set(std::string Key, std::string Val);
};