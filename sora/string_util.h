// Ŭnicode please 
#pragma once

#include <unordered_map>
#include "string_hash.h"

namespace sora {;

template<typename T=char>
struct SID {
	typedef T value_type;

	SID() : hash(0), str(nullptr), length(0) {}
	SID(unsigned int hash, const T *str, int len)
		: hash(hash), str(str), length(len) {}
	unsigned int hash;
	const T *str;
	int length;

	bool operator==(const SID &o) const
	{
		return this->hash == o.hash;
	}
	bool operator!=(const SID &o) const
	{
		return !(*this == o);
	}

	static_assert(std::is_same<char, T>::value == 1 || std::is_same<wchar_t, T>::value == 1, "not valid sid type");
};

template<typename T>
class StringTable {
	typedef T value_type;
	typedef typename T::value_type elem_type;
	typedef CRC32 HashFunc;
	typedef std::unordered_map<unsigned int, SID<elem_type>* > TableType;

public:
	StringTable() {}
	~StringTable() { Clear(); }

	SID<elem_type> Get(const elem_type *str, int len)
	{
		unsigned int hash = HashFunc::Hash(str, len);
		auto found = table_.find(hash);
		if(found != table_.end()) {
			SID<elem_type> sid = *found->second;
			return sid;
		}
		// 문자열은 복사해서 추가로 생성한다. 이렇게하면
		// 모든걸 이쪽 클래스로 떠넘길수 있다.
		// 여기에서 할당된 메모리는 소멸자에서 적절히 처리하면 메모리 누수는 없다
		const elem_type *new_str = new elem_type[len];
		SID<elem_type> *sid = new SID<elem_type>(hash, new_str, len);
		table_[hash] = sid;
		return *sid;
	}

	SID<elem_type> Get(const T &str)
	{
		return Get(str.data(), str.length());
	}

	bool Clear()
	{
		for(auto it : table_) {
			SID<elem_type> *sid = it.second;
			delete[](sid->str);
			delete(sid);
		}
		table_.clear();
		return true;
	}

	static StringTable &Global()
	{
		static StringTable ctx;
		return ctx;
	}

private:
	TableType table_;
};

class StringUtil {
public:
	template<typename T>
	static const T &WhiteSpaceString(Loki::Type2Type<T>);
	static const std::string &WhiteSpaceString(Loki::Type2Type<std::string>)
	{
		static std::string str("\t\n\x0b\x0c\r ");
		return str;
	}
	static const std::wstring &WhiteSpaceString(Loki::Type2Type<std::wstring>)
	{
		static std::wstring str(L"\t\n\x0b\x0c\r ");
		return str;
	}

	template<typename T>
	static T Trim(const T &str) 
	{
		using std::size_t;

		// do no use boost to compile speed + dependenty
		const typename T::value_type *whitespace = WhiteSpaceString(Loki::Type2Type<T>()).data();

		size_t leftFound = str.find_first_not_of(whitespace);
		size_t rightFound = str.find_last_not_of(whitespace);

		if (leftFound == T::npos) {
			leftFound = 0;
		}
		if (rightFound == T::npos) {
			rightFound = str.length();
		}
		T result = str.substr(leftFound, rightFound-leftFound+1);
		return result;
	}

	template<typename T>
	static T LeftTrim(const T &str) 
	{
		using std::size_t;
		const typename T::value_type *whitespace = WhiteSpaceString(Loki::Type2Type<T>()).data();
		size_t n = str.find_first_not_of(whitespace);
		T left(n == T::npos ? str : str.substr(n, str.length()));
		return left;
	}

	template<typename T>
	static T RightTrim(const T &str) 
	{
		using std::size_t;
		const typename T::value_type *whitespace = WhiteSpaceString(Loki::Type2Type<T>()).data();
		size_t n = str.find_last_not_of(whitespace);
		T right(n == T::npos ? str : str.substr(0, n + 1));
		return right;
	}

	template<typename T>
	static T Join(const T &str, const std::vector<T> &tokenlist) 
	{
		//typedef basic_ostringstream<char, char_traits<char>, allocator<char> > ostringstream;
		typename typedef T::value_type elem_type;
		typedef std::basic_ostringstream<elem_type, std::char_traits<elem_type>, std::allocator<elem_type> > stringstream_type;

		stringstream_type oss;
		for (size_t i = 0 ; i < tokenlist.size() ; i++) {
			oss << tokenlist[i];
			if (i != tokenlist.size() - 1) {
				oss << str;
			}
		}
		return oss.str();
	}

	template<typename T>
	static int Split(const T &str, typename T::value_type ch, std::vector<T> *retval) 
	{
		//SR_ASSERT(retval->empty() == true);
		// if no delimeter
		if (str.length() == 0) {
			retval->push_back(str);
			return retval->size();
		}

		// simple impl
		T tmp_token;
		for (size_t i = 0 ; i < str.length() ; i++) {
			typename T::value_type str_ch = str[i];
			if (str_ch == ch) {
				// split point
				retval->push_back(tmp_token);
				tmp_token.clear();
			} else {
				tmp_token.push_back(str_ch);
			}
		}
		retval->push_back(tmp_token);
		return retval->size();
	}

};

}   // namespace sora