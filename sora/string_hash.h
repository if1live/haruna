// Ŭnicode please
#pragma once

#include <string>
#include "template_lib.h"

namespace sora {;
//잡다한 함수 있어봣자 어차피 CRC밖에 안쓴다

template<typename T>
class CRC32Logic : public Singleton<CRC32Logic<T>> {
public:
    CRC32Logic() { GenerateTable(); }
    ~CRC32Logic() {}
	///@brief calculate the crc value
	unsigned int Hash(const T *str, int len) const
	{
		unsigned int crc = 0xFFFFFFFF;
		for(int i = 0 ; i < len ; i++) {
			int ch = static_cast<int>(str[i]);
			crc = (crc>>8) ^ crc_table[ (crc^ch) & 0xFF ];
		}     
		return( crc^0xFFFFFFFF );
	}

private:
	///@brief build the crc table 
	void GenerateTable()
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
				crc_table[i] = crc;
			}
		}
	}
	unsigned int crc_table[256];
};

struct CRC32 {
	template<typename T>
    static unsigned int Hash(const T *str, int len)
	{
		return CRC32Logic<T>::GetInstance().Hash(str, len);
	}
};

}   // namespace sora