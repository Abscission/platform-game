
//Copyright (c) 2015 Jason Light
//License: MIT

//This header contains the memory management code for Abscission's platform game engine.

//TODO: Add useful allocators (pools etc)
//TODO: Add memory free functions

#pragma once

#include "Types.h"

class MemoryManager {
public:
	///<summary>Allocates memory aligned to n bytes</summary>
	///<param name="Length">The number of bytes to allocate</param>
	///<param name="Alignment">The number of bytes the memory should be aligned to</param>
	static byte* AllocateMemory(size_t Length);

	template <typename t>
	static t* AllocateMemory(size_t Count = 1);

	static void FreeMemory(void* Start);
};

template <typename t>
t* MemoryManager::AllocateMemory(size_t Count) {
	return reinterpret_cast<t*>(AllocateMemory(Count * sizeof(t)));
}

template <typename t>
class Pool {
public:
	struct Node {
		int next;
		t item;
	};

	Node* Nodes;
	size_t Count;

	int First = 0;

	Pool(size_t Count) {
		Nodes = MemoryManager::AllocateMemory<Node>(Count);

		for (int i = 0; i < Count; i++) {
			Nodes[i].next = i + 1;
		}
	}

	~Pool() {
		MemoryManager::FreeMemory((void*)Nodes);
	}

	t* Get() {
		Node* n = &Nodes[First];
		First = n->next;
		return new (&n->item) t();
	}
};
