// Ŭnicode please
#pragma once

#include <string>

namespace sora {;
class HeapAllocHeader;
class Heap;

enum {
	kAllocatedMemorySignature = 0x12345678,
};

class Heap {
public:
	typedef unsigned char byte;

public:
	Heap();
	explicit Heap(const std::string &name);
	~Heap();

	const std::string &name() const { return name_; }
	int id() const { return id_; }

	void* Malloc(size_t size);
	void Free(void *ptr);

	static Heap *GetSharedHeap();
	int GetAllocatedMemSize() const;

	template<typename T>
	static HeapAllocHeader *GetHeader(const T *ptr);

private:
	void AddAllocation(size_t size);
	void RemoveAllocation(size_t size);

private:
	int id_;
	std::string name_;
	int allocated_size_;
	static int next_id_;
};

struct HeapAllocHeader {
public:
	explicit HeapAllocHeader()
		: signature(kAllocatedMemorySignature),
		size(0),
		heap(NULL) {
	}
	~HeapAllocHeader() {}
	void Reset(int signature, int size, Heap *heap) {
		this->signature = signature;
		this->size = size;
		this->heap = heap;
	}

public:
	int signature;
	int size;
	Heap *heap;
};

//impl
template<typename T>
HeapAllocHeader *Heap::GetHeader(const T *ptr) 
{
	HeapAllocHeader *header = reinterpret_cast<HeapAllocHeader*>(
		(byte*)(ptr) - sizeof(HeapAllocHeader));
	return header;
}
} //namespace sora
