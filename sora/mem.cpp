// Ŭnicode please 
#include "stdafx.h"
#include "mem.h"

namespace sora {;

//for tag allocator
struct TagAllocHeader {
	TagAllocHeader()
		: prev(NULL),
		next(NULL),
		magic(TagAllocator::kMagicNumber),
		tag(0),
		size(0) 
	{
		prev = this;
		next = this;
	}

	TagAllocHeader *prev;
	TagAllocHeader *next;
	unsigned short magic;
	unsigned short tag;
	int size;
};

//for basic allocator
AllocState basic_alloc_state;

std::array<AllocState, TagAllocator::kMaxTag> tag_alloc_state_table;  //tag별로 할당상태 별로 관리
std::array<TagAllocHeader, TagAllocator::kMaxTag> mem_chain;  //tag는 총 16개

bool MEM_Init()
{
	basic_alloc_state = AllocState();
	for(int i = 0 ; i < TagAllocator::kMaxTag ; i++) {
		TagAllocator::Flush(i);
		tag_alloc_state_table[i] = AllocState();
	}
	return true;
}

bool Mem_Deinit()
{
	basic_alloc_state = AllocState();
	for(int i = 0 ; i < TagAllocator::kMaxTag ; i++) {
		TagAllocator::Flush(i);
		tag_alloc_state_table[i] = AllocState();
	}
	return false;
}

///////////////////////////

void *global_malloc(size_t x) 
{
	return malloc(x);
}
void global_free(void *p) 
{
	return free(p);
}

struct BasicAllocHeader {
	//경계정보 같은거 없음. 그냥 size만 추가해서 메모리 할당을 추적할수 잇도록함
	BasicAllocHeader() : size(0) { }
	BasicAllocHeader(int size) : size(size) { }
	int size;
};

AllocState &BasicAllocator::alloc_state() 
{
	AllocState &state = basic_alloc_state;
	return state;
}

void *BasicAllocator::Malloc(size_t size) {
	size_t request_size = size + sizeof(BasicAllocHeader);
	void *ptr = sora::global_malloc(request_size);
	BasicAllocHeader *header = (BasicAllocHeader*)ptr;
	header->size = size;
	void *result = (void*)((long)ptr + sizeof(BasicAllocHeader));

	basic_alloc_state.bytes += size;
	basic_alloc_state.count++;

	return result;
}
void BasicAllocator::Free(void *ptr) {
	void *alloc_start = (void*)((long)ptr - sizeof(BasicAllocHeader));
	BasicAllocHeader *header = (BasicAllocHeader*)alloc_start;

	int size = header->size;
	basic_alloc_state.bytes -= size;
	basic_alloc_state.count--;

	sora::global_free(header);
}

void *BasicAllocator::Calloc( size_t num, size_t size ) {
	int request_size = num * size;
	unsigned char *ptr = static_cast<unsigned char*>(Malloc(request_size));
	std::fill(ptr, ptr + request_size, 0);
	return ptr;
}

void *BasicAllocator::Realloc( void *memblock, size_t size ) {
	void *alloc_start = (void*)((long)memblock - sizeof(BasicAllocHeader));
	BasicAllocHeader *prev_header = (BasicAllocHeader*)alloc_start;

	int prev_size = prev_header->size;

	void *new_block = ::realloc(alloc_start, size);
	BasicAllocHeader *new_header = (BasicAllocHeader*)new_block;
	new_header->size = size;
	basic_alloc_state.bytes += (size - prev_size);

	void *result = (void*)((long)new_block + sizeof(BasicAllocHeader));
	return result;
}

AllocState &TagAllocator::tag_alloc_state(int tag) 
{
	SR_ASSERT(tag >= 0);
	SR_ASSERT(tag < kMaxTag);
	return tag_alloc_state_table[tag];
}

void TagAllocator::Free(void *ptr) 
{
	void *alloc_start = (void*)((long)ptr - sizeof(TagAllocHeader));
	TagAllocHeader *header = (TagAllocHeader*)alloc_start;

	SR_ASSERT(header->magic == kMagicNumber && "Free::BadMagic!");

	header->prev->next = header->next;
	header->next->prev = header->prev;

	unsigned short tag = header->tag;
	tag_alloc_state_table[tag].bytes -= header->size;
	tag_alloc_state_table[tag].count--;

	sora::global_free(alloc_start);
	return;
}

void *TagAllocator::Malloc(size_t size, int tag) 
{
	SR_ASSERT(tag >= 0);
	SR_ASSERT(tag < kMaxTag);

	size_t request_size = size + sizeof(TagAllocHeader);
	void *ptr = sora::global_malloc(request_size);

	TagAllocHeader *header = (TagAllocHeader*)ptr;
	header->magic = kMagicNumber;
	header->tag = tag;
	header->size = size;

	TagAllocHeader &root = mem_chain[tag];
	header->prev = root.prev;
	header->next = &root;
	root.prev->next = header;
	root.prev = header;

	tag_alloc_state_table[tag].bytes += header->size;
	tag_alloc_state_table[tag].count++;

	void *result = (void*)((long)ptr + sizeof(TagAllocHeader));
	return result;
}

void TagAllocator::Flush(int tag) 
{
	SR_ASSERT(tag >= 0);
	SR_ASSERT(tag < kMaxTag);

	TagAllocHeader *header = mem_chain[tag].next;
	TagAllocHeader *next = NULL;
	for ( ; header != &mem_chain[tag] ; header = next) {
		next = header->next;
		if (header->tag == tag) {
			void *alloc_ptr = (void*)((long)header + sizeof(TagAllocHeader));
			Free(alloc_ptr);
		}
	}
}

void TagAllocator::TotalAllocStat(AllocState *data) 
{
	memset(data, 0, sizeof(AllocState));
	for(int i = 0 ; i < kMaxTag ; i++) {
		data->bytes += tag_alloc_state_table[i].bytes;
		data->count += tag_alloc_state_table[i].count;
	}
}

}	// namespace sora