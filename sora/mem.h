// Ŭnicode please 
#pragma once

namespace sora {;

bool MEM_Init();
bool Mem_Deinit();

inline void *global_malloc(size_t x);
inline void global_free(void *p);

template<typename T>
inline T* global_malloc() 
{
	return reinterpret_cast<T*>(global_malloc(sizeof(T)));
}

template<typename T>
inline T* global_new() 
{
	T *ptr = global_malloc<T>();
	new(ptr) T;
	return ptr;
}
template<typename T, typename P1>
inline T* global_new(P1 &p1) 
{
	T *ptr = global_malloc<T>();
	new(ptr) T(p1);
	return ptr;
}
template<typename T, typename P1, typename P2>
inline T* global_new(P1 &p1, P2 &p2) 
{
	T *ptr = global_malloc<T>();
	new(ptr) T(p1, p2);
	return ptr;
}

template<typename T, typename P1, typename P2, typename P3>
inline T* global_new(P1 &p1, P2 &p2, P3 &p3) 
{
	T *ptr = global_malloc<T>();
	new(ptr) T(p1, p2, p3);
	return ptr;
}

template<typename T, typename P1, typename P2, typename P3, typename P4>
inline T* global_new(P1 &p1, P2 &p2, P3 &p3, P4 &p4) 
{
	T *ptr = global_malloc<T>();
	new(ptr) T(p1, p2, p3, p4);
	return ptr;
}

template<typename T>
inline void global_delete(T *ptr) {
	ptr->~T();
	global_free(ptr);
}

// memory alloc stat
struct AllocState {
	AllocState() : bytes(0), count(0) {}
	int bytes;
	int count;
};

// low level alloc, support only simple log
// replace basic alloc
class BasicAllocator {
public:
	static void *Malloc(size_t size);
	static void Free(void *ptr);
	static void *Calloc( size_t num, size_t size );
	static void *Realloc( void *memblock, size_t size );
	static AllocState &alloc_state();
};

// add header alloc. tag계열에서 삭제
class TagAllocator {
public:
	enum {
		kMagicNumber = 1234,
		kMaxTag = 16,
	};
	static void *Malloc(size_t size, int tag);
	static void Free(void *ptr);
	static void Flush(int tag);  //동일한 tag를 달고잇는거 한방에 삭제
	static AllocState &tag_alloc_state(int tag);
	static void TotalAllocStat(AllocState *data);
};


}	// namespace sora