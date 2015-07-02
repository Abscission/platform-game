
#include "Level.h"

#include "Types.h"
#include "Utility.h"
#include "MemoryManager.h"

Level::Level() {
	Chunks = (Chunk*)MemoryManager::AllocateMemory(sizeof(Chunk) * UINT16_MAX, 0);
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
	Chunks[LookupLocation(X, Y)] = C;
}

Chunk& Level::GetChunk(u16 X, u16 Y){
	return Chunks[LookupLocation(X, Y)];
}