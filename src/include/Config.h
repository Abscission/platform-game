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
	std::string Get(std::string Key);
	std::string Get(std::string Key, std::string Default);

	bool Set(std::string Key, std::string Val);
};