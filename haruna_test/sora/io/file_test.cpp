﻿// Ŭnicode please
#include "test_stdafx.h"
#include "sora/io/file.h"
#include "sora/io/memory_file.h"

using sora::io::WriteonlyCFile;
using sora::io::ReadonlyCFile;
using sora::io::MemoryFile;
using sora::io::FS_Init;
using sora::io::FS_Deinit;


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
/*
TEST_F(FileTest, LowLevelCFile_test) 
{
	const char *path1 = "low_level_c_file.txt";

	LowLevelCFile file1(path1);
	EXPECT_EQ(false, file1.IsOpened());
	ASSERT_EQ(true, file1.Open("rb"));

	EXPECT_EQ(28, file1.GetLength());
	const char *content = "this is line1.this is line2.";
	const char *buffer = (const char *)file1.GetBuffer();
	EXPECT_STREQ(content, buffer);
}
*/
TEST_F(FileTest, WriteonlyCFile_ReadonlyCFile_test) 
{
	const char path1[] = "asdf.txt";
	const char content[] = "this is line1.this is line2.";

	WriteonlyCFile file1(path1);
	EXPECT_EQ(false, file1.IsOpened());
	ASSERT_EQ(true, file1.Open());
	file1.Write(content, sizeof(content));
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
	const char *path1 = "low_level_c_file.txt";

	MemoryFile file1(path1);
	EXPECT_EQ(false, file1.IsOpened());
	ASSERT_EQ(true, file1.Open());

	EXPECT_EQ(28, file1.GetLength());
	const char *content = "this is line1.this is line2.";
	EXPECT_STREQ(content, (const char*)file1.start);
}