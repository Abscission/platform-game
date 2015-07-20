#pragma once

#include "MemoryManager.h"

template <typename t>
class DoubleLinkedList {
public:
	struct Node {
		t* Item;
		Node* Next;
		Node* Previous;
	};

	Node* First;
	Node* Last;

	DoubleLinkedList() {
		First = nullptr;
		Last = nullptr;
	}

	Node* Insert(t* Item) {
		//Allocate memory for the node, make this a pool allocator?
		Node* n = MemoryManager::AllocateMemory<Node>();

		//Set up the node
		n->Item = Item;
		n->Next = nullptr;
		n->Previous = Last;

		//Add the node into the list
		if (Last != nullptr) Last->Next = n;

		//And adjust the list itself
		if (First == nullptr) First = n;
		Last = n;

		return n;
	}

	void Remove(Node* n){
		//TODO: Check if the node is in the list?
		if (n->Previous) {
			n->Previous->Next = n == Last ? nullptr : n->Next;
		}
		
		if (n->Next) {
			n->Next->Previous = n == First ? nullptr : n->Previous;
		}

		if (n == Last) Last = n->Previous;
		if (n == First) First = n->Next;
	}
};