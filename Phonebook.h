/* ----------------------------------------
 * Author: Sandeep Kalra
 * License: MIT License.
 * Copyright(c) 2013 Sandeep.Kalra@gmail.com
 */
#ifndef __PHONEBOOK_H_
#define __PHONEBOOK_H_


#include <iostream>
#include <map>
#include <string>
using namespace std;

// ref: http://www.gotw.ca/gotw/029.htm
struct ci_char_traits : public char_traits<char>
	// just inherit all the other functions
	//  that we don't need to override
{
	static bool eq(char c1, char c2)
	{
		return toupper(c1) == toupper(c2);
	}

	static bool ne(char c1, char c2)
	{
		return toupper(c1) != toupper(c2);
	}

	static bool lt(char c1, char c2)
	{
		return toupper(c1) <  toupper(c2);
	}

	static int compare(const char* s1,
		const char* s2,
		size_t n) {
		return _memicmp(s1, s2, n); // Linux: memicmp ; Windows: _memicmp
	}

	static const char*
		find(const char* s, int n, char a) {
			while (n-- > 0 && toupper(*s) != toupper(a)) {
				++s;
			}
			return s;
		}
};

// this class does case-insensitive comparision
typedef basic_string <char, ci_char_traits> String;


//Note: This class is not thread-safe!
class CPhonebook {
	map<String, String> entry;
	bool overwrite_on_insert = false;
	bool size_unlimited = false;
	int max_sz = 100;
	CPhonebook() = default;

public:


	~CPhonebook() { entry.erase(begin(entry), end(entry)); }
	CPhonebook(const CPhonebook&) = delete; // no copy allowed;
	CPhonebook(CPhonebook&&) = delete; // no move allowed;
	
	static CPhonebook& GetInstance()	{
		static CPhonebook instance;
		return instance;
	}
	
	void SetPolicies(bool v_overwrite_on_insert = false, bool v_size_unlimited = false, int v_max_sz=100) 	{ 
		// This function must be called at the start of usage of the CPhonebook, else may result in errors.
		overwrite_on_insert = v_overwrite_on_insert; 
		size_unlimited = v_size_unlimited; 
		if (!size_unlimited) max_sz = v_max_sz;
	}
	

	bool Insert (const String &name, const String &phone_or_uri)	{
		auto i = entry.find(name);		
		if (overwrite_on_insert && i != entry.end() /* valid entry found */) {
			i->second = phone_or_uri;
			return true;
		}
		else if (i == entry.end()) {
			if (!size_unlimited && entry.size() >= max_sz) return false; //full
			entry.insert(pair<String, String>(name, phone_or_uri));
			return true;
		}
		else return false;
	}

	bool Delete (const String &name)	{
		auto i = entry.find(name);
		if (i == entry.end()) return false;
		else
			entry.erase(i);
		return true;
	}

	bool lkup(bool by_name, const String &search, String &answer)	{
		for (auto i = entry.begin(); i != entry.end(); ++i)
		{
			if (by_name && i->first == search) { answer = i->second; return true; }
			else if (!by_name && i->second == search) { answer = i->first; return true; }
		}
		return false;
	}
};

#endif
