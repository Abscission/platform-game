#include "MemoryManager.h"

#include <Windows.h>

byte* MemoryManager::AllocateMemory(size_t Length) {
	byte* Memory = (byte*)VirtualAlloc(0, Length, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	return Memory;
}

void MemoryManager::FreeMemory(void * Start)
{
	VirtualFree(Start, 0, MEM_RELEASE);
}