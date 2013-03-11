// Ŭnicode please
#pragma once

namespace sora {;

class StackAllocator {
public:
	typedef long Marker;

	StackAllocator(int stack_size = 1024 * 64); //64kb
	~StackAllocator();

	void *Malloc(size_t x);
	void Free(void *data);

	void FreeToMarker(Marker marker);
	Marker GetMarker();

	void Clear();

	int stack_size() const { return stack_size_; }
	int remain_size() const { return stack_size_ - (top_ - data_); }
	int GetAllocHeaderSize() const;

private:
	unsigned char *data_;
	unsigned char *top_;
	int stack_size_;

	//메모리 할당 정보를 적절히 추적해서 디버깅이나 기타 용도에 쓸수잇을듯?
	//할당 크기는 메모리 할당크기+Header를 합쳐서 치자. (이게 진짜 할당 크기니까)
};


class DoubleStackAllocator {
public:
	typedef long Marker;
	DoubleStackAllocator(int stack_size = 1024 * 64); //64kb
	~DoubleStackAllocator();

	//스택의 아래쪽은 그냥 스택할당자와 같은 기능이다
	void* LowerMalloc(size_t x);
	void LowerFree(void *ptr);

	void* UpperMalloc(size_t x);
	void UpperFree(void *ptr);

	void LowerClear();
	void UpperClear();

	void FreeToLowerMarker(Marker marker);
	void FreeToUpperMarker(Marker marker);
	Marker GetLowerMarker();
	Marker GetUpperMarker();

	int stack_size() const { return stack_size_; }
	int remain_size() const;
	int GetAllocHeaderSize() const;

private:
	unsigned char *data_;
	unsigned char *lower_;
	unsigned char *upper_;
	int stack_size_;
};

} //namespace sora
