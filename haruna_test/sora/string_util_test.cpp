// Ŭnicode please 
#include "test_stdafx.h"
#include "sora/string_util.h"

using std::string;
using std::wstring;
using std::vector;
using sora::StringUtil;

TEST(StringTable, Get)
{
	sora::StringTable table;

	sora::SID a = table.Get("this is a");
	sora::SID b = table.Get("this is a", 9);
	sora::SID c = table.Get(std::string("this is a"));
	sora::SID d = table.Get("asddf");

	EXPECT_TRUE(a == b);
	EXPECT_TRUE(a == c);
	EXPECT_TRUE(a != d);
}

TEST(StringUtil, Trim) 
{
	string a("as df");
	EXPECT_TRUE(StringUtil::Trim(a) == "as df");
	string b("  as df");
	EXPECT_TRUE(StringUtil::Trim(b) == "as df");
	string c("as df  ");
	EXPECT_TRUE(StringUtil::Trim(c) == "as df");
	string d("  as df  ");
	EXPECT_TRUE(StringUtil::Trim(d) == "as df");
}

TEST(StringUtil, LeftTrim) 
{
	string a("as df");
	EXPECT_TRUE(StringUtil::LeftTrim(a) == "as df");
	string b("  as df");
	EXPECT_TRUE(StringUtil::LeftTrim(b) == "as df");
	string c("as df  ");
	EXPECT_TRUE(StringUtil::LeftTrim(c) == "as df  ");
	string d("  as df  ");
	EXPECT_TRUE(StringUtil::LeftTrim(d) == "as df  ");
}

TEST(StringUtil, RightTrim) 
{
	string a("as df");
	EXPECT_TRUE(StringUtil::RightTrim(a) == "as df");
	string b("  as df");
	EXPECT_TRUE(StringUtil::RightTrim(b) == "  as df");
	string c("as df  ");
	EXPECT_TRUE(StringUtil::RightTrim(c) == "as df");
	string d("  as df  ");
	EXPECT_TRUE(StringUtil::RightTrim(d) == "  as df");
}


TEST(StringUtil, Split) 
{
	vector<string> result;
	int retval;

	//없는거 쪼개기 시도
	string test0("kldp.org");
	retval = sora::StringUtil::Split(test0, 'z', &result);
	EXPECT_EQ(1, retval);

	//1글자로 자르기
	string test1("kldp.org/node/11");
	result.clear();
	retval = sora::StringUtil::Split(test1, '/', &result);
	EXPECT_EQ(3, retval);
	ASSERT_EQ(3, (int)result.size());
	EXPECT_TRUE(result[0] == "kldp.org");
	EXPECT_TRUE(result[1] == "node");
	EXPECT_TRUE(result[2] == "11");	
}

TEST(StringUtil, Join) 
{
	string delim("::");
	string result;

	vector<string> strlist0;
	strlist0.push_back("a");
	strlist0.push_back("b");
	strlist0.push_back("c");
	result = StringUtil::Join(delim, strlist0);
	EXPECT_TRUE(result == "a::b::c");
}
