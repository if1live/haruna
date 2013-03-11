// Ŭnicode please 
#pragma once

#include <unordered_map>
#include "string_hash.h"

namespace sora {;

struct SID {
    SID() : hash(0), str(nullptr), length(0) {}
    SID(unsigned int hash, const char *str, int len)
        : hash(hash), str(str), length(len) {}
    unsigned int hash;
    const char *str;
    int length;

    bool operator==(const SID &o) const;
    bool operator!=(const SID &o) const;
};

class StringTable {
    typedef std::unordered_map<unsigned int, SID*> TableType;
    typedef APHash HashFunc;
public:
    StringTable();
    ~StringTable();

    SID Get(const char *str);
    SID Get(const char *str, int len);
    SID Get(const std::string &str);

    static StringTable &Global();
private:
    TableType table_;
};


}   // namespace sora