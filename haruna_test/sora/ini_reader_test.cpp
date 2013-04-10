// Ŭnicode please 
#include "test_stdafx.h"
#include "sora/ini_reader.h"
#include "sora/io/file.h"

using namespace sora;
using namespace sora::io;

TEST(INIReader, run)
{
	ReadonlyCFile file("test.ini");
	file.Open();
	
	const char *data = static_cast<const char*>(file.GetBuffer());
	std::string content(data, file.GetLength());

	INIReader reader(content);

	EXPECT_EQ(0, reader.ParseError());
	EXPECT_EQ(6, reader.GetInteger("protocol", "version", -1));
	EXPECT_STREQ("Bob Smith", reader.Get("user", "name", "UNKNOWN").data());
	EXPECT_STREQ("bob@smith.com", reader.Get("user", "email", "UNKNOWN").data());
	EXPECT_EQ(true, reader.GetBoolean("user", "active", true));
}
