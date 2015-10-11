
//Copyright (c) 2015 Jason Light
//License: MIT

#pragma once

#include <string>
#include <Windows.h>
#include "Types.h"

//See http://blog.abscission.net/asset-management/ for info on development

//This pragma ensures the values are packed as tightly as possible, regardless of ideal alignment
//This allows us to more accurately predict how the written files will look
#pragma pack(push, 1)
struct Header {
	//The 4 character file identifier
	//This should be equal to "AGEA"
	char FileID[4];

	//The number of entries in this asset file
	int NumberOfEntries;

	//The length of the data section
	//This should be the compressed size if compression is enabled
	int DataLength;
};

struct IndexEntry {
	//The 12 character name for the asset. Will be used in the level editor
	char Name[12];

	//The position of the asset as an offset from the start of the data section
	int Position;

	//The length of the asset (compressed length if applicable)
	int Length;

	//Is the data compressed
	bool Compressed;
};
#pragma pack(pop)

//This struct is a container for returning from the GetAsset function
//It has a destructor to destroy the asset when it falls out of scope to prevent memory leaks
struct Asset {
	void* Memory;
	int Length;
	~Asset();
};


//The main AssetFile class
class AssetFile {
private:
	Header* FileHeader;
	IndexEntry* Indexes;
	HANDLE File;
	HANDLE FileMapping;

public:
	u64 NumberOfAssets;
	std::string Filename;

	AssetFile(char* Filename);
	~AssetFile();

	Asset GetAsset(int FileID);
};