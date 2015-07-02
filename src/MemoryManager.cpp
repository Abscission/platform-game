#include "MemoryManager.h"

#include <Windows.h>

byte* MemoryManager::AllocateMemory(size_t Length, u32 Alignment) {
	byte* Memory = (byte*)VirtualAlloc(0, Length + Alignment, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	return Memory;
}