// Ŭnicode please
#pragma once

#include <string>

//http://blog.daum.net/aswip/4538253
//문자열 해쉬함수 모음 (String Hash Function) 
namespace sora {;
struct RSHash {
    static unsigned int Hash(const char *str, int len);
};

struct JSHash {
    static unsigned int Hash(const char *str, int len);
};

// P. J. Weinberger Hash Function
struct PJWHash {
    static unsigned int Hash(const char *str, int len);
};

struct ELFHash {
    static unsigned int Hash(const char *str, int len);
};


struct BKDRHash {
    static unsigned int Hash(const char *str, int len);
};

struct SDBMHash {
    static unsigned int Hash(const char *str, int len);
};

struct DJBHash {
    static unsigned int Hash(const char *str, int len);
};

struct DEKHash {
    static unsigned int Hash(const char *str, int len);
};

struct APHash {
    static unsigned int Hash(const char *str, int len);
};

struct CRC32 {
    static unsigned int Hash(const char *str, int len);
};

}   // namespace sora