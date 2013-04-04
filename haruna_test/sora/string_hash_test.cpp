// Ŭnicode please
#include "test_stdafx.h"
#include "sora/string_hash.h"

using sora::CRC32;

TEST(StringHash, CRC32)
{
    //http://zorc.breitbandkatze.de/crc.html
    //crc 계산기를 이용해서 테스트값을 확인함
    //테스트값
    //CRC order : 32
    //CRC polynom : 4C11DB7
    //initial value : ffffffff
    //final xor value : ffffffff
	CRC32<char> crc;

	std::string str = "abc";
    unsigned int value = crc.Hash(str.data(), str.length());
	EXPECT_EQ(0x352441c2u, value);
	
	str = "asdf1234";
	value = crc.Hash(str.data(), str.length());
	EXPECT_EQ(0x9b5345e4u, value);

	//static version + wchar
	std::wstring wstr = L"asdf1234";
	value = CRC32<wchar_t>::GetHash(wstr.data(), wstr.length());
	EXPECT_EQ(0x9b5345e4u, value);
}