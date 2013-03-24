// Ŭnicode please 
#include "test_stdafx.h"
#include <array>
#include "sora/io/zip_helper.h"

using std::vector;
using std::array;
using sora::io::ContainerStream;
using sora::io::ZipHelper;

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


TEST(ZipHelper, Run)
{
	vector<unsigned char> src;
	src.push_back('a');
	src.push_back('b');
	src.push_back('c');
	src.push_back('d');
	src.push_back('e');

	vector<unsigned char> dst;

	ZipHelper helper;
	EXPECT_TRUE(helper.Compress(src, &dst));

	vector<unsigned char> data;
	EXPECT_TRUE(helper.Decompress(dst, &data));
	
	EXPECT_TRUE(src.size() == data.size());
	EXPECT_TRUE(std::equal(src.begin(), src.end(), data.begin()));
}