
//Copyright (c) 2015 Jason Light
//License: MIT

#include "Utility.h"

#include "List.h"
#include "Test.h"
#include "LogManager.h"

void Test()
{
	  //////////////////////////////////
	 //  MEMORY ALLOCATION TEST CODE //
	//////////////////////////////////

	int* test = MemoryManager::AllocateMemory<int>();
	assert(!IsBadWritePtr(test, sizeof(int)), "Memory allocation test failed: allocate");

	MemoryManager::FreeMemory(test);
	assert(IsBadWritePtr(test, sizeof(int)), "Memory free test failed: free");

	  ////////////////////////////////////
	 //  DOUBLY LINKED LIST TEST CODE  //
	////////////////////////////////////

	int* testint = MemoryManager::AllocateMemory<int>(10);
	testint[0] = 5;
	testint[1] = 12;
	testint[2] = 32;
	testint[3] = -2;
	testint[4] = 124125;

	DoubleLinkedList<int> TestList;
	DoubleLinkedList<int>::Node* testnode = TestList.Insert(&testint[0]);
	TestList.Insert(&testint[1]);
	TestList.Insert(&testint[2]);
	TestList.Insert(&testint[3]);
	TestList.Insert(&testint[4]);

	assert(*testnode->Item == 5, "Doubly linked list test failed: set");
	assert(*testnode->Next->Item == 12, "Doubly linked list test failed: next");

	TestList.Remove(testnode);


	///////////////////////////////
	//  POOL ALLOCATOR TEST CODE //
	///////////////////////////////

	//Create a pool of floats with length 10
	Pool<float>* TestPool = new Pool<float>(10);
	assert(TestPool != nullptr, "P ool allocator fails");

	//Get three values from the pool
	float* first = TestPool->Get();
	float* second = TestPool->Get();
	float* third = TestPool->Get();

	//Set their values
	*second = 2.f;
	*third = 123.4f;

	//Delete the pool
	delete TestPool;

	//////////////////////
	// LOGING TEST CODE //
	//////////////////////
	
	Log TestLog("TestLog.txt");
	TestLog.Write("Hello!");
	TestLog.Write("I am a cheese");



}
