// Ŭnicode please 
#include "test_stdafx.h"
#include <array>
#include "sora/io/zip_helper.h"

#include <Windows.h>
#include "sora/io/filesystem.h"

using std::vector;
using std::array;
using std::string;
using sora::io::ContainerStream;
using sora::io::ZlibHelper;
using sora::io::ZipHelper;

using sora::io::FS_Init;
using sora::io::FS_Deinit;

TEST(ContainerStream, Read)
{
	{
		vector<unsigned char> src;
		src.push_back('a');
		src.push_back('b');
		src.push_back('c');
		src.push_back('d');
		src.push_back('e');

		ContainerStream<vector<unsigned char>> in_stream(src);
	
		const int chunk = 2;
		unsigned char buf[chunk];
		int read_retval = 0;

		read_retval = in_stream.Read(chunk, buf);
		EXPECT_EQ(2, read_retval);
		EXPECT_EQ(false, in_stream.IsEnd());
		EXPECT_EQ('a', buf[0]);
		EXPECT_EQ('b', buf[1]);

		read_retval = in_stream.Read(chunk, buf);
		EXPECT_EQ(2, read_retval);
		EXPECT_EQ(false, in_stream.IsEnd());
		EXPECT_EQ('c', buf[0]);
		EXPECT_EQ('d', buf[1]);

		read_retval = in_stream.Read(chunk, buf);
		EXPECT_EQ(1, read_retval);
		EXPECT_EQ(true, in_stream.IsEnd());
		EXPECT_EQ('e', buf[0]);

		read_retval = in_stream.Read(chunk, buf);
		EXPECT_EQ(0, read_retval);
	}
	{
		array<unsigned char, 2> src = {'a', 'b'};
		ContainerStream<array<unsigned char, 2>> in_stream(src);
	
		array<unsigned char, 2> buf;
		int read_retval = 0;

		read_retval = in_stream.Read(buf.size(), buf);
		EXPECT_EQ(2, read_retval);
		EXPECT_EQ('a', buf[0]);
		EXPECT_EQ('b', buf[1]);
	}
}

class ZipHelperTest : public ::testing::Test {
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

TEST_F(ZipHelperTest, Run)
{
	vector<unsigned char> src;
	src.push_back('a');
	src.push_back('b');
	src.push_back('c');
	src.push_back('d');
	src.push_back('e');

	vector<unsigned char> dst;

	ZlibHelper helper;
	EXPECT_TRUE(helper.Compress(src, &dst));

	vector<unsigned char> data;
	EXPECT_TRUE(helper.Decompress(dst, &data));
	
	EXPECT_TRUE(src.size() == data.size());
	EXPECT_TRUE(std::equal(src.begin(), src.end(), data.begin()));
}

TEST_F(ZipHelperTest, Zip)
{
	std::string zipfilename = "zlib127-dll.zip";
	std::string password = "";

	ZipHelper helper;
	EXPECT_TRUE(helper.Open(zipfilename, password));
	helper.PrintList();

	vector<string> file_list = helper.GetList();
	vector<string> expect_file_list;
	expect_file_list.push_back("DLL_FAQ.txt");
	expect_file_list.push_back("README.txt");
	expect_file_list.push_back("USAGE.txt");
	expect_file_list.push_back("zlib1.dll");
	expect_file_list.push_back("include/zconf.h");
	expect_file_list.push_back("include/zlib.h");
	expect_file_list.push_back("lib/zdll.lib");
	expect_file_list.push_back("lib/zlib.def");
	expect_file_list.push_back("test/example_d.exe");
	expect_file_list.push_back("test/minigzip_d.exe");

	EXPECT_TRUE(file_list.size() == expect_file_list.size());
	EXPECT_TRUE(std::equal(file_list.begin(), file_list.end(), expect_file_list.begin()));

	vector<unsigned char> data;
	bool retval = false;
	
	retval = helper.GetFile("DLL_FAQ.txt", &data);
	EXPECT_EQ(retval, true);
	EXPECT_EQ(17921, data.size());

	data.clear();
	retval = helper.GetFile("USAGE.txt", &data);
	EXPECT_EQ(retval, true);
	EXPECT_EQ(2895, data.size());
}
