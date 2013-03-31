// Ŭnicode please
#include "stdafx.h"
#include "heap.h"
#include "mem.h"

namespace sora {;
int Heap::next_id_ = 1;
Heap::Heap()
	: id_(next_id_++), name_(""), allocated_size_(0) 
{
}

Heap::Heap(const std::string &name)
	: id_(next_id_++), name_(name), allocated_size_(0) 
{
}

Heap::~Heap() 
{
}

Heap *Heap::GetSharedHeap() 
{
	using std::string;
	static Heap h(string("default"));
	return &h;
}

void* Heap::Malloc(size_t size) 
{
#if _DEBUG
	int requestSize = size + sizeof(HeapAllocHeader);
	byte *mem = reinterpret_cast<byte*>(sora::global_malloc(requestSize));
	HeapAllocHeader *header = reinterpret_cast<HeapAllocHeader*>(mem);
	header->Reset(kAllocatedMemorySignature, size, this);
	this->AddAllocation(size);

	void *startBlock = mem + sizeof(HeapAllocHeader);
	return startBlock;
#else
	byte *mem = reinterpret_cast<byte*>(sora::global_malloc(size));
	return mem;
#endif
}
void Heap::Free(void *ptr) 
{
#if _DEBUG
	HeapAllocHeader *header = reinterpret_cast<HeapAllocHeader*>(
		reinterpret_cast<byte*>(ptr) - sizeof(HeapAllocHeader));
	header->heap->RemoveAllocation(header->size);
	SR_ASSERT(header->signature == kAllocatedMemorySignature);
	sora::global_free(header);
#else
	sora::global_free(ptr);
#endif
}

void Heap::AddAllocation(size_t size) 
{
	allocated_size_ += size;
}
void Heap::RemoveAllocation(size_t size) 
{
	allocated_size_ -= size;
}
int Heap::GetAllocatedMemSize() const {
	return allocated_size_;
}

}	// namespace sora
