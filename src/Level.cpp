
#include "Level.h"

#include <vector>

#include "Types.h"
#include "Maths.h"
#include "Utility.h"
#include "MemoryManager.h"

Level::Level() {
	Chunks = (Chunk**)MemoryManager::AllocateMemory(sizeof(Chunk*) * UINT16_MAX);
	Sprites = (Sprite**)MemoryManager::AllocateMemory(sizeof(Sprite*) * UINT8_MAX);
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

void Level::SetChunk(u16 X, u16 Y, Chunk C){
	C.X = X;
	C.Y = Y;

	Chunk** Parent = &Chunks[LookupLocation(X, Y)];
	while (*Parent != NULL) {
		Parent = &(*Parent)->Collission;
	}
	Chunk* ToSet = new Chunk();

	*Parent = ToSet;
	memcpy((void*)ToSet, &C, sizeof(C));
}

Chunk* Level::GetChunk(u16 X, u16 Y){
	Chunk* Location = Chunks[LookupLocation(X, Y)];
	if (Location == nullptr) {
		return new Chunk{ X, Y, 0 };
	}
	while (Location->X != X || Location->Y != Y) {
		if (Location->Collission == nullptr) {
			return nullptr;
		}
		Location = Location->Collission;
	}
	return Location;
}

#define ChunkLoc(x,y) (y*16+x)

std::vector<iRect> Level::GenerateCollisionGeometryFromChunk(u16 X, u16 Y) {
	Chunk* C = GetChunk(X, Y);

	std::vector<iRect> CollisionGeometry;

	if (C == nullptr) {
		return CollisionGeometry;
	}

	bool Visited[16 * 16] = {};

	for (int y = 0; y < 16; y++) {
		for (int x = 0; x < 16; x++) {
			if (C->Grid[ChunkLoc(x, y)].Collision &!Visited[ChunkLoc(x, y)]) {
				int Top = (y + Y * 16) * 32;
				int Left = (x + X * 16) * 32;
				int Width = 32;

				int n = 0;
				while (C->Grid[ChunkLoc(x + (++n), y)].Collision) {
					Visited[ChunkLoc(x + n, y)] = true;
					Width += 32;
				}

				CollisionGeometry.push_back({ Left, Top, Width, 32} );
			}
		}
	}

	return CollisionGeometry;
}

void Level::UpdateChunk(u16 X, u16 Y, double DeltaTime, std::vector<iRect>& Geometry) {
	Chunk* C = GetChunk(X, Y);

	if (C == nullptr) {
		return;
	}

	auto Entity = C->Entities.First;

	while (Entity != nullptr) {
		Entity->Item->Update(DeltaTime, Geometry);
		Entity = Entity->Next;
	}
}

void Level::DrawChunk(Renderer* Renderer, u16 X, u16 Y) {
	Chunk* C = GetChunk(X, Y);

	if (C == nullptr) {
		return;
	}

	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			if (C->Grid[y * 16 + x].Texture != NULL) {
				Renderer->DrawSprite(Sprites[C->Grid[y * 16 + x].Texture], 32 * (16 * X + x), 32 * (16 * Y + y));
			}
		}
	}

	auto Entity = C->Entities.First;

	while (Entity != nullptr) {
		Entity->Item->Draw(Renderer);
		Entity = Entity->Next;
	}
	

	/*for (auto Entity : C->Entities) {
		if (Entity != nullptr) {
			Entity->Draw(Renderer);
		}
	}*/
}