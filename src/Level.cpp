
#include "Level.h"

#include <vector>

#include "Types.h"
#include "Maths.h"
#include "Utility.h"
#include "MemoryManager.h"

Level::Level() {
	Chunks = (Chunk**)MemoryManager::AllocateMemory(sizeof(Chunk*) * UINT16_MAX, 0);
	Sprites = (Sprite**)MemoryManager::AllocateMemory(sizeof(Sprite*) * UINT8_MAX, 0);
}

Level::~Level() {
	VirtualFree((void*)Chunks, sizeof(Chunk) * UINT16_MAX, 8);
}

//Essentially a hash funciton for our hash table
u32 Level::LookupLocation(u16 X, u16 Y) {
	//TODO: Make this less bad ;-)
	//We need a hash function which minimizes collisisons

	return (X * UINT16_MAX + Y) % UINT16_MAX;
}

void Level::SetChunk(u16 X, u16 Y, Chunk C){
	Chunk** Location = &Chunks[LookupLocation(X, Y)];
	while (*Location != NULL) {
		*Location = ((*Location)->Collission);
	}
	*Location = new Chunk();
	memcpy((void*)*Location, &C, sizeof(C));
}

Chunk* Level::GetChunk(u16 X, u16 Y){
	Chunk** Location = &Chunks[LookupLocation(X, Y)];

	return *Location;
}

std::vector<iRect> Level::GenerateCollisionGeometryFromChunk(u16 X, u16 Y) {
	Chunk C = *(GetChunk(X, Y));

	std::vector<iRect> CollisionGeometry;

	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			if (C.Grid[Y * 16 + X].Collision) {
				CollisionGeometry.push_back({ (x + X * 16) * 32, (y + Y * 16) * 32, 32, 32 } );
			}
		}
	}

	return CollisionGeometry;
}

void Level::DrawChunk(Renderer* Renderer, u16 X, u16 Y) {
	Chunk C = *(GetChunk(X, Y));

	for (X = 0; X < 16; X++) {
		for (Y = 0; Y < 16; Y++) {
			if (C.Grid[Y * 16 + X].Texture != NULL) {
				Renderer->DrawSprite(Sprites[C.Grid[Y * 16 + X].Texture], 32 * X, 32 * Y);
			}
		}
	}
}