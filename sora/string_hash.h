// Ŭnicode please
#pragma once

#include <string>
#include <array>

namespace sora {;
//잡다한 함수 있어봣자 어차피 CRC밖에 안쓴다

template<typename T>
class HashBase {
public:
	virtual unsigned int Hash(const T *str, int len) = 0;
};

template<typename T>
class APHash : public HashBase<T> {
public:
	static unsigned int GetHash(const T *str, int len)
	{
		return APHash().Hash(str, len);
	}
	// AP Hash Function
	unsigned int Hash(const T *str, int length) 
	{
		unsigned int hash = 0;
		for(int i = 0; i < length; i++) {
			hash ^= ((i & 1) == 0) ? (  (hash <<  7) ^ str[i] ^ (hash >> 3)) :
			(~((hash << 11) ^ str[i] ^ (hash >> 5)));
		}
		return (hash & 0x7FFFFFFF);
	}
};

template<typename T>
class RSHash : public HashBase<T> {
public:
	static unsigned int GetHash(const T *str, int len)
	{
		return RSHash().Hash(str, len);
	}
	unsigned int Hash(const T *str, int length)
	{
		unsigned int b = 378551;
		unsigned int a = 63689;
		unsigned int hash = 0;
		for(int i = 0; i < length; i++) {
			hash = hash * a + str[i];
			a = a * b;
		}
		return (hash & 0x7FFFFFFF);
	}
};

template<typename T>
class CRC32 : public HashBase<T> {	
public:
	CRC32() { BuildTable(); }

	static unsigned int GetHash(const T *str, int len)
	{
		static CRC32 hash_obj;
		return hash_obj.Hash(str, len);
	}

    unsigned int Hash(const T *str, int len)
	{
		unsigned int crc = 0xFFFFFFFF;
		for(int i = 0 ; i < len ; i++) {
			int ch = static_cast<int>(str[i]);
			crc = (crc>>8) ^ crc_table_[ (crc^ch) & 0xFF ];
		}     
		return( crc^0xFFFFFFFF );
	}

	bool BuildTable()
	{
		unsigned int crc, poly;
		int i, j;

		poly = 0xEDB88320L;
		for (i = 0; i < 256; i++) {
			crc = i;
			for (j = 8; j > 0; j--) {
				if (crc & 1) {
					crc = (crc >> 1) ^ poly;
				} else {
					crc >>= 1;
				}
				crc_table_[i] = crc;
			}
		}
		return true;
	}

private:
	std::array<unsigned int, 256> crc_table_;
};

}   // namespace sora