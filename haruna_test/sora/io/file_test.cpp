// Ŭnicode please
#include "test_stdafx.h"
#include "sora/io/file.h"
#include "sora/io/memory_file.h"

using sora::io::WriteonlyCFile;
using sora::io::ReadonlyCFile;
using sora::io::MemoryFile;
using sora::io::FS_Init;
using sora::io::FS_Deinit;
using std::string;

class FileTest : public ::testing::Test {
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

TEST_F(FileTest, ReadonlyCFile_test) 
{
	string path1 = "low_level_c_file.txt";

	ReadonlyCFile file1(path1);
	EXPECT_EQ(false, file1.IsOpened());
	ASSERT_EQ(true, file1.Open());

	EXPECT_EQ(28, file1.GetLength());
	const char *content = "this is line1.this is line2.";
	const char *buffer = (const char *)file1.GetBuffer();
	EXPECT_STREQ(content, buffer);
}


TEST_F(FileTest, WriteonlyCFile_ReadonlyCFile_test) 
{
	string path1 = "WriteonlyCFile_ReadonlyCFile_test.tmp";
	const char content[] = "this is line1.this is line2.";

	WriteonlyCFile file1(path1);
	EXPECT_EQ(false, file1.IsOpened());
	ASSERT_EQ(true, file1.Open());
	int retval = file1.Write(content, sizeof(content));
	EXPECT_EQ(retval, sizeof(content));
	file1.Close();

	ReadonlyCFile file2(path1);
	EXPECT_EQ(false, file2.IsOpened());
	EXPECT_EQ(true, file2.Open());
	EXPECT_EQ(sizeof(content), file2.GetLength());
	const char *buffer = (const char *)file2.GetBuffer();
	EXPECT_STREQ(content, buffer);
}

TEST_F(FileTest, MemoryFile_test) 
{
	string path1 = "low_level_c_file.txt";

	MemoryFile file1(path1);
	EXPECT_EQ(false, file1.IsOpened());
	ASSERT_EQ(true, file1.Open());

	EXPECT_EQ(28, file1.GetLength());
	const char *content = "this is line1.this is line2.";
	EXPECT_STREQ(content, (const char*)file1.start);
}

TEST_F(FileTest, ReadonlyCFile_ReadContainer)
{
	// read의 인자를 썡 배열로 사용할수도 있지만
	// std::array라든가 std::vector도 쓸수잇으면 좋겟다
	string path1 = "low_level_c_file.txt";
	ReadonlyCFile file1(path1);
	EXPECT_EQ(false, file1.IsOpened());
	ASSERT_EQ(true, file1.Open());

	//TODO
}

TEST_F(FileTest, WriteonlyCFile_WriteContainer) 
{
	// write 인자를 썡 배열로 사용할수도 있지만
	// std::array라든가 std::vector도 쓸수있으면 좋겠다
	string path1 = "WriteonlyCFile_WriteContainer.tmp";
	unsigned char data1[] = "this is line1.this is line2.";
	const int data_length = sizeof(data1);
	std::vector<unsigned char> data2(data_length);
	std::array<unsigned char, data_length> data3 = { '\0', };
	for(int i = 0 ; i < data_length ; ++i) {
		data2[i] = data1[i];
		data3[i] = data1[i];
	}

	WriteonlyCFile file1(path1);
	int retval = 0;

	EXPECT_EQ(false, file1.IsOpened());
	ASSERT_EQ(true, file1.Open());
	retval = file1.WriteContainer(data2);
	EXPECT_EQ(data_length, retval);
	file1.Close();
	
	EXPECT_EQ(false, file1.IsOpened());
	ASSERT_EQ(true, file1.Open());
	retval = file1.WriteContainer(data3);
	EXPECT_EQ(data_length, retval);
	file1.Close();
}

TEST_F(FileTest, ReadonlyCFile_MemoryFile_Seek)
{
	using sora::io::ReadableFile;

	string path1 = "low_level_c_file.txt";
	const char *expected = "this is line1.this is line2.";
	
	typedef std::unique_ptr<ReadableFile> ReadableFilePtr;
	std::vector<ReadableFilePtr> file_list;
	//file_list.push_back(std::unique_ptr<ReadableFile>(new ReadonlyCFile(path1)));
	file_list.push_back(std::unique_ptr<ReadableFile>(new MemoryFile(path1)));

	for(size_t i = 0 ; i < file_list.size() ; ++i) {
		auto f = file_list[i].get();

		EXPECT_EQ(false, f->IsOpened());
		ASSERT_EQ(true, f->Open());

		char ch = '\0';
		EXPECT_TRUE(f->Seek(0, sora::io::kSeekStart));
		EXPECT_EQ(1, f->Read(&ch, 1));
		EXPECT_EQ('t', ch);

		EXPECT_TRUE(f->Seek(4, sora::io::kSeekCurr));
		EXPECT_EQ(1, f->Read(&ch, 1));
		EXPECT_EQ('i', ch);

		EXPECT_TRUE(f->Seek(-1, sora::io::kSeekEnd));
		EXPECT_EQ(1, f->Read(&ch, 1));
		EXPECT_EQ('.', ch);
	}
}

TEST_F(FileTest, ReadonlyCFile_MemoryFile_Read)
{
	using sora::io::ReadableFile;

	string path1 = "low_level_c_file.txt";
	const char *expected = "this is line1.this is line2.";

	typedef std::unique_ptr<ReadableFile> ReadableFilePtr;
	std::vector<ReadableFilePtr> file_list;
	file_list.push_back(std::unique_ptr<ReadableFile>(new ReadonlyCFile(path1)));
	file_list.push_back(std::unique_ptr<ReadableFile>(new MemoryFile(path1)));

	for(size_t i = 0 ; i < file_list.size() ; ++i) {
		ReadableFile *f = file_list[i].get();

		EXPECT_EQ(false, f->IsOpened());
		ASSERT_EQ(true, f->Open());

		std::array<unsigned char, 128> buf;

		std::fill(buf.begin(), buf.end(), '\0');
		EXPECT_EQ(4, f->Read(buf.data(), 4));
		EXPECT_STREQ((const char*)"this", (const char*)buf.data());
		
		std::fill(buf.begin(), buf.end(), '\0');
		EXPECT_EQ(20, f->Read(buf.data(), 20));
		EXPECT_STREQ((const char*)" is line1.this is li", (const char*)buf.data());

		std::fill(buf.begin(), buf.end(), '\0');
		EXPECT_EQ(4, f->Read(buf.data(), 10));
		EXPECT_STREQ((const char*)"ne2.", (const char*)buf.data());

		std::fill(buf.begin(), buf.end(), '\0');
		EXPECT_EQ(0, f->Read(buf.data(), 4));
	}	
}
