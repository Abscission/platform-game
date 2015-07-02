#pragma once

#include "Types.h"

class MemoryManager {
public:
	///<summary>Allocates memory aligned to n bytes</summary>
	///<param name="Length">The number of bytes to allocate</param>
	///<param name="Alignment">The number of bytes the memory should be aligned to</param>
	static byte* AllocateMemory(size_t Length, u32 Alignment);
};