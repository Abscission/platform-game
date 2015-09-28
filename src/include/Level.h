
//Copyright (c) 2015 Jason Light
//License: MIT

#include "Types.h"
#include <vector>

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

	DoubleLinkedList<GameObject> Entities;
	//GameObject* Entities[32];

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

	void SetChunk(u16 X, u16 Y, Chunk* C);
	Chunk* GetChunk(u16 X, u16 Y);

	std::vector<iRect> GenerateCollisionGeometryFromChunk(u16 X, u16 Y);

	void LoadFromAsset(Asset asset);

	void UpdateChunk(u16 X, u16 Y, double DeltaTime, std::vector<iRect>& Geometry);

	void SpawnEntity(GameObject* Object, u32 X, u32 Y);

	void DrawChunk(Renderer* Renderer, u16 X, u16 Y);

	void DrawChunkEntities(Renderer* Renderer, u16 X, u16 Y);
};
