// Ŭnicode please
#include "test_stdafx.h"
#include "sora/io/filesystem.h"

using sora::io::FS_Init;
using sora::io::FS_Deinit;

class FilesystemTest : public ::testing::Test {
protected:
	virtual void SetUp()
	{
		FS_Init();
	}
	virtual void TearDown()
	{
		FS_Deinit();
	}
};

TEST_F(FilesystemTest, GetExtension) 
{
	using std::string;
	using sora::io::Filesystem;

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