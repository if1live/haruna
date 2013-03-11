// Ŭnicode please 
#include "test_stdafx.h"
#include "sora/string_util.h"

TEST(StringTable, Get)
{
	sora::StringTable table;

	sora::SID a = table.Get("this is a");
	sora::SID b = table.Get("this is a", 9);
	sora::SID c = table.Get(std::string("this is a"));
	sora::SID d = table.Get("asddf");

	EXPECT_TRUE(a == b);
	EXPECT_TRUE(a == c);
	EXPECT_TRUE(a != d);
}