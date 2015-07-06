#include "AssetManager.h"

#include <Windows.h>
#include <zlib.h>

AssetFile::AssetFile(char* Filename) {
	//WinAPI functions to create and open a file mapping. At the end, FileHeader is a pointer to the start of the file
	File = CreateFileA(Filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	FileMapping = CreateFileMapping(File, NULL, PAGE_READONLY, 0, 0, NULL);
	FileHeader = (Header*)MapViewOfFile(FileMapping, FILE_MAP_READ, 0, 0, 0);
	
		//Check if the file contains our identifier, "AGEA":
	if (*(int*)&FileHeader->FileID != 0x41454741) {
		OutputDebugStringA("Invalid!");
	}

	Indexes = (IndexEntry*)((char*)FileHeader + 13);

	Data = (char*)Indexes + FileHeader->NumberOfEntries * sizeof(IndexEntry);
}

AssetFile::~AssetFile() {
	UnmapViewOfFile(FileHeader);
}

Asset AssetFile::GetAsset(int FileID){
	if (FileID > FileHeader->NumberOfEntries) return{ 0, 0 };

	IndexEntry IE = Indexes[FileID];

	if (!FileHeader->Compressed) {
			//Copy the file
		Asset Asset = {};
		Asset.Memory = VirtualAlloc(0, IE.Length, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (!Asset.Memory) {
			return{ 0, 0 };
		}
		memcpy(Asset.Memory, Data + IE.Position, IE.Length);
		Asset.Length = IE.Length;
		return Asset;
	}
	else {
		//Decompress the file
		//Grab the variables from the data section
		char* Data = this->Data + IE.Position;
		unsigned int OriginalSize = *(int*)Data;
		unsigned int CompressedSize = *((int*)Data + 1);
		Data = (char*)((int*)Data + 2);

		//Create the asset
		Asset Asset;// = new Asset();
		Asset.Memory = VirtualAlloc(0, OriginalSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

		if (!Asset.Memory) {
			return{ 0, 0 };
		}

		uncompress((Byte*)Asset.Memory, (uLongf*)&OriginalSize, (Byte*)Data, CompressedSize);

		Asset.Length = OriginalSize;
		return Asset;
	}
}

Asset::~Asset() {
	//if (Memory) VirtualFree(Memory, 0, MEM_RELEASE);
}