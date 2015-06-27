#pragma once

#include <Windows.h>

namespace AssetManager {
#pragma pack(push, 1)
	struct Header {
		char FileID[4];

		int NumberOfEntries;
		int DataLength;

		bool Compressed;
	};

	struct IndexEntry {
		char Name[12];
		int Position;
		int Length;
	};
#pragma pack(pop)

	struct Asset {
		void* Memory;
		int Length;
		~Asset();
	};

	class AssetFile {
	private:
		Header* FileHeader;
		IndexEntry* Indexes;
		char* Data;

		HANDLE File;
		HANDLE FileMapping;

	public:
		AssetFile(char* Filename);
		~AssetFile();

		Asset GetAsset(int FileID);
	};
}