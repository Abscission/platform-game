
//Copyright (c) 2015 Jason Light
//License: MIT

#include "MemoryManager.h"
#include <Windows.h>
#include <intrin.h>
#include <immintrin.h>
#include "Utility.h"

byte* MemoryManager::AllocateMemory(size_t Length) {
	byte* Memory = (byte*)VirtualAlloc(0, Length, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	return Memory;
}

void MemoryManager::FreeMemory(void * Start)
{
	VirtualFree(Start, 0, MEM_RELEASE);
}

void abscission_memcpy(void* dst, void* src, uP len) {
	if ((len % 32 == 0) && InstructionSet::AVX()) {
		//Use AVX to copy 256 bits at a time
		len /= 32;
		for (register uP i = 0; i < len; i++)
			_mm256_stream_si256(&(((__m256i*)dst)[i]), ((__m256i*)src)[i]);
	}
	else {
		memcpy(dst, src, len);
	}
}