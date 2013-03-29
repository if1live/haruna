// Ŭnicode please
#include "test_stdafx.h"
#include "sora/io/filesystem.h"

#include <filesystem>

using sora::io::FS_Init;
using sora::io::FS_Deinit;

namespace fs = std::tr2::sys;
using std::string;
using std::vector;
using sora::io::Filesystem;

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
	const char file1[] = "asdf.txt";
	EXPECT_STREQ(".txt", Filesystem::GetExtension(file1).c_str());
	const char file2[] = "ad/sdf/asdf.txt";
	EXPECT_STREQ(".txt", Filesystem::GetExtension(file2).c_str());
	const char file3[] = "ad\\sdf\\asdf.txt";
	EXPECT_STREQ(".txt", Filesystem::GetExtension(file3).c_str());
	const char file4[] = "sdf";
	EXPECT_STREQ("", Filesystem::GetExtension(file4).c_str());
	const char file5[] = "ad/sd.f/asdf.txt";
	EXPECT_STREQ(".txt", Filesystem::GetExtension(file5).c_str());
	const char file6[] = "ad/sd.f/asdf";
	EXPECT_STREQ("", Filesystem::GetExtension(file6).c_str());
}

TEST_F(FilesystemTest, GetFileList)
{
	vector<string> no_file_list = Filesystem::GetFileList("not_exist");	

	vector<string> file_list = Filesystem::GetFileList(".");	
	for(const string &path : file_list) {
		std::cout << path << std::endl;
	}

	EXPECT_TRUE(file_list.end() != std::find(file_list.begin(), file_list.end(), "low_level_c_file.txt"));
	EXPECT_TRUE(file_list.end() != std::find(file_list.begin(), file_list.end(), "zlib127-dll.zip"));		
}