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

class StringUtil {
public:
	template<typename T>
	static T Trim(const T &str) 
	{
		using std::size_t;

		// do no use boost to compile speed + dependenty
		const typename T::value_type *whitespace = "\t\n\x0b\x0c\r ";

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
		size_t n = str.find_first_not_of("\t\n\x0b\x0c\r ");
		T left(n == T::npos ? str : str.substr(n, str.length()));
		return left;
	}

	template<typename T>
	static T RightTrim(const T &str) 
	{
		using std::size_t;
		size_t n = str.find_last_not_of("\t\n\x0b\x0c\r ");
		T right(n == T::npos ? str : str.substr(0, n + 1));
		return right;
	}

	template<typename T>
	static T Join(const T &str, const std::vector<T> &tokenlist) 
	{
		std::ostringstream oss;
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