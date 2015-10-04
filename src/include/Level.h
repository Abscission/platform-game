
//Copyright (c) 2015 Jason Light
//License: MIT

#include "Types.h"
#include <vector>
#include <string>
#include <list>

#include "Types.h"
#include "List.h"
#include "Renderer.h"
#include "GameObject.h"

struct GridSquare {
	u16 Texture;
	u8 Collision;
};

struct Chunk {
	u16 X;
	u16 Y;
	GridSquare Grid[16*16];

	bool inIndex;

	std::vector<iRect>* Geometry;

	Chunk* Collission;
};

class Level {
private:

public:

	//These may be private when done!
	
	std::vector<Sprite> Sprites;
	u32 LookupLocation(u16 X, u16 Y);
	Chunk** Chunks;

	//Public here

	std::string Name;
	std::string Author;
	std::string Filename;

	std::vector<std::string> AssetFiles;
	std::list<std::vector<int>> AssetIndices;

	std::vector<IVec2> ExistingChunks;
	DoubleLinkedList<GameObject> Entities;

	IVec2 GetChunkFromLocation(IVec2 Location) {
		return{ Location.X / 16, Location.Y / 16 };
	}

	//Constructor and destructor for the level
	Level();
	~Level();

	//The width and height of the level
	u16 Width;
	u16 Height;

	Chunk* SetChunk(u16 X, u16 Y, Chunk* C);
	Chunk* GetChunk(u16 X, u16 Y);

	std::vector<iRect> GenerateCollisionGeometryFromChunk(u16 X, u16 Y);
	std::vector<iRect> GenerateCollisionGeometryFromChunk(Chunk* C);

	void SetChunkGeometry(Chunk* C);

	void LoadFromAsset(Asset asset);

	void Update(double DeltaTime, std::vector<IVec2>& Geometry);

	void UpdateChunk(u16 X, u16 Y, double DeltaTime, std::vector<iRect>& Geometry);

	void SpawnEntity(GameObject* Object, u32 X, u32 Y);

	void DrawChunk(Renderer* Renderer, u16 X, u16 Y);

	void DrawChunkCollisionGeometry(Renderer* Renderer, u16 X, u16 Y);
	void Save();
};
