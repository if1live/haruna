// Ŭnicode please
#include <gtest/gtest.h>
#include "sora/filesystem.h"

TEST(Filesystem, GetExtension) 
{
	using std::string;
	using sora::Filesystem;

	const char file1[] = "asdf.txt";
	EXPECT_STREQ("txt", Filesystem::GetExtension(file1).c_str());
	const char file2[] = "ad/sdf/asdf.txt";
	EXPECT_STREQ("txt", Filesystem::GetExtension(file2).c_str());
	const char file3[] = "ad\\sdf\\asdf.txt";
	EXPECT_STREQ("txt", Filesystem::GetExtension(file3).c_str());
	const char file4[] = "sdf";
	EXPECT_STREQ("", Filesystem::GetExtension(file4).c_str());
	const char file5[] = "ad/sd.f/asdf.txt";
	EXPECT_STREQ("txt", Filesystem::GetExtension(file5).c_str());
	const char file6[] = "ad/sd.f/asdf";
	EXPECT_STREQ("", Filesystem::GetExtension(file6).c_str());
}