// Ŭnicode please
#include "test_stdafx.h"
#include "sora/mem.h"

using namespace sora;

class MemTest : public ::testing::Test {
protected:
	virtual void SetUp()
	{
		MEM_Init();
	}
	virtual void TearDown()
	{
		Mem_Deinit();
	}
};

class BasicAllocatorTest : public MemTest {};
class TagAllocatorTest : public MemTest {};

TEST_F(BasicAllocatorTest, malloc_free) 
{
	using sora::AllocState;
	using namespace sora;

	//empty
	AllocState &state = BasicAllocator::alloc_state();
	ASSERT_EQ(0, state.bytes);
	ASSERT_EQ(0, state.count);

	void *mem1 = BasicAllocator::Malloc(128);
	EXPECT_EQ(128, state.bytes);
	EXPECT_EQ(1, state.count);

	void *mem2 = BasicAllocator::Malloc(64);
	EXPECT_EQ(128+64, state.bytes);
	EXPECT_EQ(2, state.count);

	void *mem3 = BasicAllocator::Malloc(128);
	EXPECT_EQ(128+64+128, state.bytes);
	EXPECT_EQ(3, state.count);

	BasicAllocator::Free(mem2);
	EXPECT_EQ(128+128, state.bytes);
	EXPECT_EQ(2, state.count);

	BasicAllocator::Free(mem1);
	EXPECT_EQ(128, state.bytes);
	EXPECT_EQ(1, state.count);

	BasicAllocator::Free(mem3);
	EXPECT_EQ(0, state.bytes);
	EXPECT_EQ(0, state.count);
}

TEST_F(BasicAllocatorTest, realloc) 
{
	using sora::AllocState;
	using namespace sora;

	//empty
	AllocState &state = BasicAllocator::alloc_state();
	EXPECT_EQ(0, state.bytes);
	EXPECT_EQ(0, state.count);

	void *mem1 = BasicAllocator::Malloc(16);
	EXPECT_EQ(16, state.bytes);
	EXPECT_EQ(1, state.count);

	mem1 = BasicAllocator::Realloc(mem1, 32);
	EXPECT_EQ(32, state.bytes);
	EXPECT_EQ(1, state.count);

	mem1 = BasicAllocator::Realloc(mem1, 64);
	EXPECT_EQ(64, state.bytes);
	EXPECT_EQ(1, state.count);

	BasicAllocator::Free(mem1);
	EXPECT_EQ(0, state.bytes);
	EXPECT_EQ(0, state.count);
}

TEST_F(TagAllocatorTest, Malloc_Free) 
{
	using sora::TagAllocator;
	sora::AllocState alloc_state;

	TagAllocator::TotalAllocStat(&alloc_state);
	EXPECT_EQ(0, alloc_state.bytes);
	EXPECT_EQ(0, alloc_state.count);

	int tag = 0x01;

	void *ptr1 = TagAllocator::Malloc(4, tag);
	TagAllocator::TotalAllocStat(&alloc_state);
	EXPECT_EQ(4, alloc_state.bytes);
	EXPECT_EQ(1, alloc_state.count);

	void *ptr2 = TagAllocator::Malloc(8, tag);
	TagAllocator::TotalAllocStat(&alloc_state);
	EXPECT_EQ(4+8, alloc_state.bytes);
	EXPECT_EQ(2, alloc_state.count);

	void *ptr3 = TagAllocator::Malloc(16, tag);
	TagAllocator::TotalAllocStat(&alloc_state);
	EXPECT_EQ(4+8+16, alloc_state.bytes);
	EXPECT_EQ(3, alloc_state.count);

	void *ptr4 = TagAllocator::Malloc(32, tag);
	TagAllocator::TotalAllocStat(&alloc_state);
	EXPECT_EQ(4+8+16+32, alloc_state.bytes);
	EXPECT_EQ(4, alloc_state.count);

	TagAllocator::Free(ptr1);  // 4byte
	TagAllocator::TotalAllocStat(&alloc_state);
	EXPECT_EQ(8+16+32, alloc_state.bytes);
	EXPECT_EQ(3, alloc_state.count);

	TagAllocator::Free(ptr4);  // 32byte
	TagAllocator::TotalAllocStat(&alloc_state);
	EXPECT_EQ(8+16, alloc_state.bytes);
	EXPECT_EQ(2, alloc_state.count);

	TagAllocator::Free(ptr2);  // 8byte
	TagAllocator::TotalAllocStat(&alloc_state);
	EXPECT_EQ(16, alloc_state.bytes);
	EXPECT_EQ(1, alloc_state.count);

	TagAllocator::Free(ptr3);  // 16byte
	TagAllocator::TotalAllocStat(&alloc_state);
	EXPECT_EQ(0, alloc_state.bytes);
	EXPECT_EQ(0, alloc_state.count);
}

TEST_F(TagAllocatorTest, Malloc_Free_1) 
{
	using sora::TagAllocator;
	sora::AllocState alloc_state;

	TagAllocator::TotalAllocStat(&alloc_state);
	EXPECT_EQ(0, alloc_state.bytes);
	EXPECT_EQ(0, alloc_state.count);

	int tag = 0x01;

	void *ptr1 = TagAllocator::Malloc(4, tag);
	TagAllocator::TotalAllocStat(&alloc_state);
	EXPECT_EQ(4, alloc_state.bytes);
	EXPECT_EQ(1, alloc_state.count);

	void *ptr2 = TagAllocator::Malloc(8, tag);
	TagAllocator::TotalAllocStat(&alloc_state);
	EXPECT_EQ(4+8, alloc_state.bytes);
	EXPECT_EQ(2, alloc_state.count);

	///////////
	TagAllocator::Free(ptr1);  // 4byte
	TagAllocator::TotalAllocStat(&alloc_state);
	EXPECT_EQ(8, alloc_state.bytes);
	EXPECT_EQ(1, alloc_state.count);

	void *ptr3 = TagAllocator::Malloc(16, tag);
	TagAllocator::TotalAllocStat(&alloc_state);
	EXPECT_EQ(8+16, alloc_state.bytes);
	EXPECT_EQ(2, alloc_state.count);

	void *ptr4 = TagAllocator::Malloc(32, tag);
	TagAllocator::TotalAllocStat(&alloc_state);
	EXPECT_EQ(8+16+32, alloc_state.bytes);
	EXPECT_EQ(3, alloc_state.count);

	TagAllocator::Free(ptr3);  // 16byte
	TagAllocator::TotalAllocStat(&alloc_state);
	EXPECT_EQ(8+32, alloc_state.bytes);
	EXPECT_EQ(2, alloc_state.count);

	TagAllocator::Free(ptr2);  // 8byte
	TagAllocator::TotalAllocStat(&alloc_state);
	EXPECT_EQ(32, alloc_state.bytes);
	EXPECT_EQ(1, alloc_state.count);

	TagAllocator::Free(ptr4);  // 32byte
	TagAllocator::TotalAllocStat(&alloc_state);
	EXPECT_EQ(0, alloc_state.bytes);
	EXPECT_EQ(0, alloc_state.count);
}

TEST_F(TagAllocatorTest, TagMalloc_TagFlush) 
{
	using sora::TagAllocator;
	sora::AllocState alloc_state;

	void *ptr1 = TagAllocator::Malloc(4, 1);
	void *ptr2 = TagAllocator::Malloc(8, 2);
	void *ptr3 = TagAllocator::Malloc(16, 1);
	void *ptr4 = TagAllocator::Malloc(32, 2);

	TagAllocator::TotalAllocStat(&alloc_state);
	EXPECT_EQ(4+8+16+32, alloc_state.bytes);
	EXPECT_EQ(4, alloc_state.count);

	TagAllocator::Flush(2);
	TagAllocator::TotalAllocStat(&alloc_state);
	EXPECT_EQ(4+16, alloc_state.bytes);
	EXPECT_EQ(2, alloc_state.count);

	TagAllocator::Flush(1);
	TagAllocator::TotalAllocStat(&alloc_state);
	EXPECT_EQ(0, alloc_state.bytes);
	EXPECT_EQ(0, alloc_state.count);
}