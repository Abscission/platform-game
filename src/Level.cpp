
//Copyright (c) 2015 Jason Light
//License: MIT

#include "Level.h"

#include <vector>
#include <fstream>

#include "Types.h"
#include "Maths.h"
#include "Utility.h"
#include "MemoryManager.h"
#include "LogManager.h"

Level::Level() {
	Chunks = (Chunk**)MemoryManager::AllocateMemory(sizeof(Chunk*) * UINT16_MAX);
	//Sprites = (Sprite**)MemoryManager::AllocateMemory(sizeof(_Sprite*) * UINT8_MAX);
}

Level::~Level() {
	VirtualFree((void*)Chunks, sizeof(Chunk) * UINT16_MAX, 8);
}

//Essentially a hash funciton for our hash table
u32 Level::LookupLocation(u16 X, u16 Y) {
	//TODO: Make this less bad ;-)
	//We need a hash function which minimizes collisisons

	return (Y * UINT16_MAX + X) % UINT16_MAX;
}

Chunk* Level::SetChunk(u16 X, u16 Y, Chunk* C) {

	//Create and initialize a new chunk
	Chunk* NewChunk = new Chunk();
	memcpy((void*)NewChunk, (void*)C, sizeof(*C));

	//NewChunk->Entities = {};

	NewChunk->X = X;
	NewChunk->Y = Y;

	//Handle hashmap collisions

	Chunk** ch = &Chunks[LookupLocation(X, Y)];

	if (*ch != NULL) {
		Chunk* par = NULL;

		while (*ch != NULL) {
			if (&(*ch)->Collission != NULL) {
				par = *ch;
			}

			ch = &(*ch)->Collission;
		}

		par->Collission = NewChunk;
	}

	*ch = NewChunk;

	bool Empty = true;
	for (int i = 0; i < 255; i++) {
		if (NewChunk->Grid[i].Collision != false || NewChunk->Grid[i].Texture != 0) {
			Empty = false;
		}
	}
	if (!Empty) {
		ExistingChunks.push_back({ X, Y });
	}

	SetChunkGeometry(*ch);

	return *ch;
}

Chunk* Level::GetChunk(u16 X, u16 Y){
	Chunk* Location = Chunks[LookupLocation(X, Y)];
	
	if (Location == nullptr) {
		Chunk* C = MemoryManager::AllocateMemory<Chunk>(1);
		return SetChunk(X, Y, C);
	}
	while (Location->X != X || Location->Y != Y) {
		if (Location->Collission == nullptr) {
  			Chunk* C = MemoryManager::AllocateMemory<Chunk>(1);
			return SetChunk(X, Y, C);
		}

		Location = Location->Collission;
	}
	return Location;
}


std::vector<iRect> Level::GenerateCollisionGeometryFromChunk(u16 X, u16 Y) {
	Chunk* C = GetChunk(X, Y);
	return GenerateCollisionGeometryFromChunk(C);
}

std::vector<iRect> Level::GenerateCollisionGeometryFromChunk(Chunk* C) {
	std::vector<iRect> CollisionGeometry;

	if (C == nullptr) {
		return{};
	}

	u16 X = C->X;
	u16 Y = C->Y;

	bool Visited[16 * 16] = {};

	for (register uP y = 0; y < 16; y++) {
		for (register uP x = 0; x < 16; x++) {
			if (C->Grid[ChunkLoc(x, y)].Collision &! Visited[ChunkLoc(x, y)]) {
				uP Top = (y + Y * 16) * 32;
				uP Left = (x + X * 16) * 32;
				uP Width = 32;
				uP Height = 32;

				int n = 1;
				while (C->Grid[ChunkLoc(x + n, y)].Collision &! Visited[ChunkLoc(x + n, y)]) {
					if (x + n >= 16) break;
					Visited[ChunkLoc(x + n, y)] = true;
					n++;
					Width += 32;
				}

				int m = 1;
				while (C->Grid[ChunkLoc(x, y + m)].Collision &! Visited[ChunkLoc(x, y + m)]) {
					if (y + m >= 16) break;

					bool wide = true;
					for (int i = 0; i < Width / 32; i++) {
						if (!C->Grid[ChunkLoc(x + i, y + m)].Collision) {
							wide = false;
							break;
						}
					}

					if (!wide) break;

					for (int i = 0; i < Width / 32; i++)
						Visited[ChunkLoc(x + i, y + m)] = true;
					m++;
					Height += 32;
				}

				CollisionGeometry.push_back({ (int)Left, (int)Top, (int)Width, (int)Height });
			}
		}
	}

	return CollisionGeometry;
}

void Level::SetChunkGeometry(Chunk* C) {
	if (C->Geometry) delete C->Geometry;
	C->Geometry = new std::vector<iRect>;
	*C->Geometry = GenerateCollisionGeometryFromChunk(C);
}

void Level::LoadInfoFromAsset(Asset asset) {
	byte* data = (byte*)asset.Memory;
	size_t fp = 0;

	if (*(u16*)&data[0] != 0x4C41) {
		GlobalLog.Write("Magic number doesn't match, aborting");
		return;
	}

	fp += 4;

	size_t NameLength = strlen((const char *)&data[fp]);
	Name = (const char *)&data[fp];

	fp += NameLength + 1;

	size_t AuthorLength = strlen((const char *)&data[fp]);
	Author = (const char *)&data[fp];

	fp += AuthorLength + 1;
}


void Level::LoadFromAsset(Asset asset) {
	byte* data = (byte*)asset.Memory;
	size_t fp = 0;

	if (*(u16*)&data[0] != 0x4C41) {
		GlobalLog.Write("Magic number doesn't match, aborting");
		return;
	}

	fp += 4;

	size_t NameLength = strlen((const char *)&data[fp]);
	Name = (const char *)&data[fp];

	fp += NameLength + 1;

	size_t AuthorLength = strlen((const char *)&data[fp]);
	Author = (const char *)&data[fp];

	fp += AuthorLength + 1;

	u32 NumberOfAssetFiles = *(u32*)&data[fp];
	fp += 4;

	Sprites.push_back({});


	for (register uP i = 0; i < NumberOfAssetFiles; i++) {
		char * AssetFileName = (char *)(data + fp);
		char str[256];
		sprintf_s(str, "Loading assets from '%s'", AssetFileName);
		GlobalLog.Write(str);

		AssetFiles.push_back(AssetFileName);

		fp += strlen(AssetFileName) + 1;

		u32 NumberOfAssets = *(u32*)&data[fp];

		fp += 4;
		
		AssetFile CurrentAssetFile(AssetFileName);

		std::vector<int>* Indices = new std::vector<int>;

		for (register uP j = 0; j < NumberOfAssets; j++) {
			Sprite spr;
			spr.Load(CurrentAssetFile, *(u32*)&data[fp]);
			
			Indices->push_back(*(u32*)&data[fp]);

			fp += 4;
			Sprites.push_back(spr);
		}

		AssetIndices.push_back(*Indices);

	}

	u32 ChunksToLoad = *(u32*)&data[fp];
	fp += 4;

	Chunk* Chunks = MemoryManager::AllocateMemory<Chunk>(ChunksToLoad);

	for (register uP i = 0; i < ChunksToLoad; i++) {
		u16 X = *(u16*)&data[fp];
		fp += 2;

		u16 Y = *(u16*)&data[fp];
		fp += 2;

		Chunk Chunk = Chunks[i];

		for (register uP j = 0; j < 256; j++) {
			Chunk.Grid[j] = { *(u16*)&data[fp], *(u8*)&data[fp + 2] };
			fp += 3;
		}

		SetChunk(X, Y, &Chunk);
		
	}
}

void Level::Update(double DeltaTime) {

	for (auto Entity : Entities) {
		Entity->Update(DeltaTime);
	}

	Entities.remove_if([](GameObject* O) { return O->destroy; });

}

void Level::UpdateChunk(u16 X, u16 Y, double DeltaTime, std::vector<iRect>& Geometry) {
	Chunk* C = GetChunk(X, Y);

	if (C == nullptr) {
		return;
	}
}

void Level::SpawnEntity(GameObject * Object, u32 X, u32 Y) {
	Object->Position = { (float)X, (float)Y };
	Object->SpawnPosition = Object->Position;

	Entities.push_back(Object);
}

void Level::DespawnEntity(GameObject * Object) {
	//Entities.Remove(Object);
}

void Level::DrawChunk(Renderer* Renderer, u16 X, u16 Y) {
	Chunk* C = GetChunk(X, Y);

	if (C == nullptr) {
		return;
	}

	for (register uP x = 0; x < 16; x++) {
		for (register uP y = 0; y < 16; y++) {
			if (C->Grid[y * 16 + x].Texture != NULL) {
				Renderer->DrawSprite(&Sprites[C->Grid[y * 16 + x].Texture], 0, 0, 32, 32, 32 * (16 * X + x), 32 * (16 * Y + y), false);
			}
		}
	}
}

void Level::FloodFill(u32 X, u32 Y, GridSquare ToFill) {
	//Get the coordinate of the chunk
	u16 ChunkX = X / 16;
	u16 ChunkY = Y / 16;

	//And the coordinate within the chunk
	u16 LocalX = X % 16;
	u16 LocalY = Y % 16;

	Chunk* Chunk = GetChunk(ChunkX, ChunkY);

	if (Chunk != nullptr) {
		FloodFill(Chunk, LocalX, LocalY, ToFill, Chunk->Grid[LocalY * 16 + LocalX]);
	}
}

void Level::FloodFill(Chunk * C, u16 LocalX, u16 LocalY, GridSquare ToFill, GridSquare Target) {
	GridSquare& S = C->Grid[LocalY * 16 + LocalX];

	if (S == ToFill) return;
	if (S != Target) return;

	S = ToFill;

	if (LocalX != 0) FloodFill(C, LocalX - 1, LocalY, ToFill, Target);
	if (LocalX != 15) FloodFill(C, LocalX + 1, LocalY, ToFill, Target);
	if (LocalY != 0) FloodFill(C, LocalX, LocalY - 1, ToFill, Target);
	if (LocalY != 15) FloodFill(C, LocalX, LocalY + 1, ToFill, Target);
}

void Level::DrawChunkCollisionGeometry(Renderer * Renderer, u16 X, u16 Y){
	Chunk* C = GetChunk(X, Y);

	if (C == nullptr) {
		return;
	}

	for (auto R : *C->Geometry) {
		Renderer->DrawRectangleBlendWS(R.X + 2, R.Y + 2, R.W - 4, R.H - 4, rgba(255, 0, 255, 128));
	}
}

void Level::Save() {
	if (Filename.length() == 0) {
		return;
	}

	std::ofstream Output(Filename, std::ofstream::binary);
	Output.clear();

	Output << 'A' << 'L' << '0' << '0' << Name << '\0' << Author << '\0';

	size_t NumberOfAssets = AssetFiles.size();

	Output.write((char*)&NumberOfAssets, 4);
	
	u32 i = 0;
	for (auto IndiceList : AssetIndices) {
		Output << AssetFiles[i] << '\0';

		NumberOfAssets = IndiceList.size();
		Output.write((char*)&NumberOfAssets, 4);

		for (u32 i : IndiceList) {
			Output.write((char*)&i, 4);
		}

		i++;
	}

	u32 NumChunks = (u32)ExistingChunks.size();
	Output.write((char*)&NumChunks, 4);

	for (auto C : ExistingChunks) {
		Chunk* Chunk = GetChunk(C.X, C.Y);
		
		Output.write((char*)&Chunk->X, 2);
		Output.write((char*)&Chunk->Y, 2);

		for (register size_t i = 0; i < 16 * 16; i++) {
			Output.write((char*)&Chunk->Grid[i].Texture, 2);
			Output.write((char*)&Chunk->Grid[i].Collision, 1);
		}
	}


	Output.flush();
	Output.close();
}