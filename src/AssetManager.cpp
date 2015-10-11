
//Copyright (c) 2015 Jason Light
//License: MIT

#include "AssetManager.h"

#include <Windows.h>
#include <zlib.h>

#include "LogManager.h"
#include "Utility.h"

AssetFile::AssetFile(char* Filename) {
	this->Filename = Filename;
	//WinAPI functions to create and open a file mapping. At the end, FileHeader is a pointer to the start of the file
	File = CreateFileA(Filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	//Check the file handle is valid
	if (File == INVALID_HANDLE_VALUE) {
		GlobalLog.WriteF("Cannot load file %s: The handle is invalid\n", Filename);
		exit(1); //Kill the whole program on error here, we need to catch these in debug
	}

	FileMapping = CreateFileMapping(File, NULL, PAGE_READONLY, 0, 0, NULL);
	//Check the file handle is valid
	if (FileMapping == INVALID_HANDLE_VALUE) {
		GlobalLog.WriteF("Cannot load file %s: Couldn't map file\n", Filename);
		exit(1);
	}


	//Open the actual mapping
	void* Mapping = MapViewOfFile(FileMapping, FILE_MAP_READ, 0, 0, sizeof(Header));

	assert(Mapping != 0, "File mapping invalid"); //Check it's valid

	FileHeader = new Header; //Create the header
	memcpy((void*)FileHeader, Mapping, sizeof(Header)); //Copy the header

	Indexes = new IndexEntry[FileHeader->NumberOfEntries]; //Create the indexes
	memcpy((void*)Indexes, (void*)((byte*)Mapping + sizeof(Header)), sizeof(IndexEntry) * FileHeader->NumberOfEntries); //Copy the indexes

	UnmapViewOfFile(Mapping); //Unmap the view

	//Check if the file contains our identifier, "AGEA":
	if (*(int*)&(FileHeader->FileID) != 0x41454741) {
		GlobalLog.WriteF("Invalid Asset File: %s\n", Filename);
	}

	this->NumberOfAssets = FileHeader->NumberOfEntries;
}

AssetFile::~AssetFile() {
}

Asset AssetFile::GetAsset(int FileID){
	if (FileID >= FileHeader->NumberOfEntries) return{ 0, 0 };

	IndexEntry IE = Indexes[FileID];

	//Map the file
	int DataOffset = sizeof(Header) + sizeof(IndexEntry) * FileHeader->NumberOfEntries;
	void* AssetMapping = (void*)((byte*)MapViewOfFile(FileMapping, FILE_MAP_READ, 0, 0, 0) + DataOffset + IE.Position);

	//Create the asset
	Asset Asset = {};

	if (!IE.Compressed) {
		//Copy the file
		Asset.Memory = VirtualAlloc(0, IE.Length, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (!Asset.Memory) {
			return{ 0, 0 };
		}

		memcpy(Asset.Memory, AssetMapping, IE.Length);
		Asset.Length = IE.Length;
	}
	else {
		//Decompress the file
		//Grab the variables from the data section
		unsigned int OriginalSize = *(int*)AssetMapping;
		unsigned int CompressedSize = *((int*)AssetMapping + 1);
		AssetMapping = (char*)((int*)AssetMapping + 2);

		Asset.Memory = VirtualAlloc(0, OriginalSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (!Asset.Memory) {
			return{ 0, 0 };
		}

		uncompress((Byte*)Asset.Memory, (uLongf*)&OriginalSize, (Byte*)AssetMapping, CompressedSize);
		Asset.Length = OriginalSize;
	}

	UnmapViewOfFile(AssetMapping);

	return Asset;
}

Asset::~Asset() {
	//if (Memory) VirtualFree(Memory, 0, MEM_RELEASE);
}