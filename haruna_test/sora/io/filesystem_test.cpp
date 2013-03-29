// Ŭnicode please
#include "test_stdafx.h"
#include "sora/io/filesystem.h"

#include <filesystem>
#include "sora/io/file.h"

using sora::io::FS_Init;
using sora::io::FS_Deinit;

namespace fs = std::tr2::sys;
using std::string;
using std::vector;
using sora::io::Filesystem;
using std::unique_ptr;
using sora::io::ReadableFile;
using sora::io::ReadableMemoryFile;

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

TEST_F(FilesystemTest, RegisterZip)
{
	EXPECT_TRUE(Filesystem::RegisterZip("zlib127-dll.zip"));
	EXPECT_TRUE(nullptr != Filesystem::GetZipHelperByElem("DLL_FAQ.txt"));
	EXPECT_TRUE(nullptr != Filesystem::GetZipHelperByElem("lib/zdll.lib"));
	EXPECT_TRUE(nullptr == Filesystem::GetZipHelperByElem("lib/not_exist"));
	
	EXPECT_FALSE(Filesystem::RegisterZip("low_level_c_file.txt"));
	EXPECT_FALSE(Filesystem::RegisterZip("notexist.txt"));
}

TEST_F(FilesystemTest, GetReadableFile_GetReadableMemoryFile)
{
	EXPECT_TRUE(Filesystem::RegisterZip("zlib127-dll.zip"));

	{
		//실제 존재하는 독립 파일
		std::unique_ptr<ReadableFile> a = Filesystem::GetReadableFile("low_level_c_file.txt");
		EXPECT_TRUE(nullptr != a.get());
		EXPECT_EQ(true, a->IsOpened());

		//실제로 없는 파일
		std::unique_ptr<ReadableFile> b = Filesystem::GetReadableFile("not_exist.txt");
		EXPECT_TRUE(nullptr == b.get());

		//zip안에 있는 파일
		std::unique_ptr<ReadableFile> c = Filesystem::GetReadableFile("zlib1.dll");
		EXPECT_TRUE(nullptr != c.get());
		EXPECT_EQ(true, c->IsOpened());
	}
	{
		//실제 존재하는 독립 파일
		std::unique_ptr<ReadableMemoryFile> a = Filesystem::GetReadableMemoryFile("low_level_c_file.txt");
		EXPECT_TRUE(nullptr != a.get());
		EXPECT_EQ(true, a->IsOpened());

		//실제로 없는 파일
		std::unique_ptr<ReadableMemoryFile> b = Filesystem::GetReadableMemoryFile("not_exist.txt");
		EXPECT_TRUE(nullptr == b.get());

		//zip안에 있는 파일
		std::unique_ptr<ReadableMemoryFile> c = Filesystem::GetReadableMemoryFile("zlib1.dll");
		EXPECT_TRUE(nullptr != c.get());
		EXPECT_EQ(true, c->IsOpened());
	}
}

TEST_F(FilesystemTest, GetFileType)
{
	EXPECT_TRUE(Filesystem::RegisterZip("zlib127-dll.zip"));

	EXPECT_EQ(sora::io::kElemFileRaw, Filesystem::GetFileType("low_level_c_file.txt"));
	EXPECT_EQ(sora::io::kElemFileNone, Filesystem::GetFileType("not_exist.txt"));
	EXPECT_EQ(sora::io::kElemFileZip, Filesystem::GetFileType("zlib1.dll"));
}