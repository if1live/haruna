﻿// Ŭnicode please 
#include "test_stdafx.h"
#include "sora/heap.h"

using namespace sora;
using namespace std;

//#if _DEBUG
TEST(HeapTest, test) 
{
	Heap heap(string("dummy_sample"));
	EXPECT_EQ(0, heap.GetAllocatedMemSize());

	void *ptr = heap.Malloc(10);
	EXPECT_EQ(10, heap.GetAllocatedMemSize());

	void *ptr1 = heap.Malloc(20);
	EXPECT_EQ(30, heap.GetAllocatedMemSize());

	heap.Free(ptr);
	EXPECT_EQ(20, heap.GetAllocatedMemSize());

	heap.Free(ptr1);
	EXPECT_EQ(0, heap.GetAllocatedMemSize());
}

TEST(HeapTest, GetHeader) 
{
	Heap heap(string("dummy_sample1"));
	EXPECT_EQ(0, heap.GetAllocatedMemSize());

	void *ptr = heap.Malloc(10);
	HeapAllocHeader *header = Heap::GetHeader(ptr);
	EXPECT_EQ(kAllocatedMemorySignature, header->signature);
	EXPECT_EQ(10, header->size);
	EXPECT_EQ(&heap, header->heap);
	heap.Free(ptr);
}
//#endif