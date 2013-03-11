// Ŭnicode please 
#include "stdafx.h"
#include "string_util.h"

namespace sora {;

bool SID::operator==(const SID &o) const 
{
    return this->hash == o.hash;
}
bool SID::operator!=(const SID &o) const 
{
    return !(*this == o);
}

StringTable::StringTable()
{
}

StringTable::~StringTable()
{
    for(auto it : table_) {
        SID *sid = it.second;
        delete[](sid->str);
        delete(sid);
    }
    table_.clear();
}

SID StringTable::Get(const char *str)
{
    int len = strlen(str);
    return Get(str, len);
}
SID StringTable::Get(const std::string &str)
{
    return Get(str.data(), str.length());
}
SID StringTable::Get(const char *str, int len)
{
    unsigned int hash = HashFunc::Hash(str, len);
    auto found = table_.find(hash);
    if(found != table_.end()) {
        SID sid = *found->second;
        return sid;
    }
    // 문자열은 복사해서 추가로 생성한다. 이렇게하면
    // 모든걸 이쪽 클래스로 떠넘길수 있다.
    // 여기에서 할당된 메모리는 소멸자에서 적절히 처리하면 메모리 누수는 없다
    const char *new_str = new char[len];
    SID *sid = new SID(hash, new_str, len);
    table_[hash] = sid;
    return *sid;
}

StringTable &StringTable::Global() 
{
    static StringTable ctx;
    return ctx;
}

}   // namespace sora