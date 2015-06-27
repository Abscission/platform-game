#include "MemoryManager.h"

#include <Windows.h>

byte* MemoryManager::AllocateMemoryAligned(size_t Length, u32 Alignment) {
	byte* Memory = (byte*)VirtualAlloc(0, Length + Alignment, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	//Memory += (uintptr_t)Memory % Alignment;
	return Memory;
}

