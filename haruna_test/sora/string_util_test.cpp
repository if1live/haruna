// Ŭnicode please 
#include "test_stdafx.h"
#include "sora/string_util.h"

using std::string;
using std::wstring;
using std::vector;
using sora::StringUtil;

TEST(SID, test)
{
	sora::SID<char> a;
	sora::SID<wchar_t> b;
	sora::SID<> c;
	//sora::SID<int> c;
}
TEST(StringTable, Get)
{
	{
		sora::StringTable<std::string> table;

		sora::SID<char> a = table.Get("this is a");
		sora::SID<char> b = table.Get("this is a", 9);
		sora::SID<char> c = table.Get(std::string("this is a"));
		sora::SID<char> d = table.Get("asddf");

		EXPECT_TRUE(a == b);
		EXPECT_TRUE(a == c);
		EXPECT_TRUE(a != d);
	}
	{
		sora::StringTable<std::wstring> table;

		sora::SID<wchar_t> a = table.Get(L"this is a");
		sora::SID<wchar_t> b = table.Get(L"this is a", 9);
		sora::SID<wchar_t> c = table.Get(std::wstring(L"this is a"));
		sora::SID<wchar_t> d = table.Get(L"asddf");

		EXPECT_TRUE(a == b);
		EXPECT_TRUE(a == c);
		EXPECT_TRUE(a != d);
	}
}

TEST(StringUtil, WhiteSpaceString)
{
	string a = StringUtil::WhiteSpaceString(Loki::Type2Type<std::string>());
	EXPECT_EQ(true, a.find(" ") != string::npos);
	wstring b = StringUtil::WhiteSpaceString(Loki::Type2Type<std::wstring>());
	EXPECT_EQ(true, b.find(L" ") != wstring::npos);
}

TEST(StringUtil, Trim) 
{
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
	{
		wstring a(L"as df");
		EXPECT_TRUE(StringUtil::Trim(a) == L"as df");
		wstring b(L"  as df");
		EXPECT_TRUE(StringUtil::Trim(b) == L"as df");
		wstring c(L"as df  ");
		EXPECT_TRUE(StringUtil::Trim(c) == L"as df");
		wstring d(L"  as df  ");
		EXPECT_TRUE(StringUtil::Trim(d) == L"as df");
	}
}

TEST(StringUtil, LeftTrim) 
{
	string a("as df");
	EXPECT_TRUE(StringUtil::LeftTrim(a) == "as df");
	string b("  as df");
	EXPECT_TRUE(StringUtil::LeftTrim(b) == "as df");

	wstring c(L"as df  ");
	EXPECT_TRUE(StringUtil::LeftTrim(c) == L"as df  ");
	wstring d(L"  as df  ");
	EXPECT_TRUE(StringUtil::LeftTrim(d) == L"as df  ");
}

TEST(StringUtil, RightTrim) 
{
	string a("as df");
	EXPECT_TRUE(StringUtil::RightTrim(a) == "as df");
	string b("  as df");
	EXPECT_TRUE(StringUtil::RightTrim(b) == "  as df");

	wstring c(L"as df  ");
	EXPECT_TRUE(StringUtil::RightTrim(c) == L"as df");
	wstring d(L"  as df  ");
	EXPECT_TRUE(StringUtil::RightTrim(d) == L"  as df");
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
	{
		wstring delim(L"-");
		wstring result;

		vector<wstring> strlist0;
		strlist0.push_back(L"a");
		strlist0.push_back(L"b");
		strlist0.push_back(L"c");
		result = StringUtil::Join(delim, strlist0);
		EXPECT_TRUE(result == L"a-b-c");
	}
}
