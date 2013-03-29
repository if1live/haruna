// Ŭnicode please
#include "test_stdafx.h"
#include "sora/io/file.h"
#include "sora/io/memory_file.h"

using sora::io::WriteonlyCFile;
using sora::io::ReadonlyCFile;
using sora::io::ReadableMemoryFile;
using sora::io::SimpleMemoryFile;
using sora::io::FS_Init;
using sora::io::FS_Deinit;
using std::string;
using sora::io::ReadableFile;

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

	void RunBasicTest(ReadableFile *f)
	{
		EXPECT_EQ(false, f->IsOpened());

		ASSERT_EQ(true, f->Open());
		EXPECT_EQ(true, f->IsOpened());

		EXPECT_EQ(true, f->Close());
		EXPECT_EQ(false, f->IsOpened());

		ASSERT_EQ(true, f->Open());
		EXPECT_EQ(28, f->GetLength());
		const char *content = "this is line1.this is line2.";
		const char *buffer = (const char *)f->GetBuffer();
		EXPECT_STREQ(content, buffer);
	}

	void RunSeekTest(ReadableFile *f)
	{

		//const char *expected = "this is line1.this is line2.";

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

	void RunReadTest(ReadableFile *f)
	{
		//const char *expected = "this is line1.this is line2.";

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
	
	void RunGetLengthTest(ReadableFile *f)
	{
		int expected_size = 141866;

		//열지 않고 파일 크기 확인하기
		EXPECT_EQ(expected_size, f->GetLength());

		//열고나서 파일 크기 확인하기
		ASSERT_EQ(true, f->Open());
		EXPECT_EQ(expected_size, f->GetLength());
	}

};


TEST_F(FileTest, ReadonlyCFile_test) 
{
	string path1 = "low_level_c_file.txt";
	ReadonlyCFile file1(path1);
	RunBasicTest(&file1);
}

TEST_F(FileTest, MemoryFile_test) 
{
	string path1 = "low_level_c_file.txt";
	SimpleMemoryFile file1(path1);
	RunBasicTest(&file1);
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



TEST_F(FileTest, ReadonlyCFile_ReadContainer)
{
	// read의 인자를 썡 배열로 사용할수도 있지만
	// std::array라든가 std::vector도 쓸수잇으면 좋겟다
	string path1 = "low_level_c_file.txt";
	ReadonlyCFile file1(path1);
	EXPECT_EQ(false, file1.IsOpened());
	ASSERT_EQ(true, file1.Open());

	//XXX
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



TEST_F(FileTest, ReadonlyCFile_Seek)
{
	string path1 = "low_level_c_file.txt";
	std::unique_ptr<ReadableFile> f(new ReadonlyCFile(path1));
	RunSeekTest(f.get());
}

TEST_F(FileTest, MemoryFile_Seek)
{
	string path1 = "low_level_c_file.txt";
	std::unique_ptr<ReadableFile> f(new SimpleMemoryFile(path1));
	RunSeekTest(f.get());
}


TEST_F(FileTest, ReadonlyCFile_Read)
{
	string path1 = "low_level_c_file.txt";
	std::unique_ptr<ReadableFile> f(new ReadonlyCFile(path1));
	RunReadTest(f.get());
}

TEST_F(FileTest, MemoryFile_Read)
{
	string path1 = "low_level_c_file.txt";
	std::unique_ptr<ReadableFile> f(new SimpleMemoryFile(path1));
	RunReadTest(f.get());
}

TEST_F(FileTest, ReadonlyCFile_GetLength)
{	
	string file = "zlib127-dll.zip";
	ReadonlyCFile file1(file);
	RunGetLengthTest(&file1);
}

TEST_F(FileTest, MemoryFile_GetLength)
{	
	string file = "zlib127-dll.zip";
	SimpleMemoryFile file1(file);
	RunGetLengthTest(&file1);
}

