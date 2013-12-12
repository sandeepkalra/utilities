#ifndef _COBJ_CLASS_H_
#define _COBJ_CLASS_H_
#include <iostream>
#include <map>
#include <algorithm>
#include <string>

using namespace std;
enum class CObjType : char{
	I8,
	U8,
	I16,
	U16,
	I32,
	U32,
	F32,
	STR,
	BOOL
};

class CObjBase{
	CObjType mType;
	public:
		CObjBase(CObjType v):mType{v} {}
		CObjType GetType() { return mType;}
};

template <class T>
class CObj:public CObjBase
{
	T mMin;
	T mMax;
	T mDefault;
	T mValue;
public:
	CObj()=delete;
	CObj(CObjType type, T val=T{},T min=T{},T max=T{},T def=T{}):
		CObjBase(type),
		mMin{min},
		mMax{max},
		mDefault{def}
	{
		if(val >= mMin && val <= mMax && mValue!=val) mValue=val;
	}
	void SetVal(T val)
	{
		if(val >= mMin && val <= mMax && mValue!=val) mValue=val;
	}
	T GetVal()
	{
		return mValue;
	}
	bool operator == (const CObj<T>& rhs)
	{
		return (rhs.mValue==mValue);
	}
	CObj& operator= (const CObj<T>& rhs)
	{
                mType = rhs.mType;
                mMin = rhs.mMin;
                mMax = rhs.mMax;
                mDefault = rhs.mDefault;
                mValue = rhs.mValue;
	}
	CObj(const CObj<T>& rhs) 
	{ 
                mType = rhs.mType;
		mMin = rhs.mMin;
		mMax = rhs.mMax;
		mDefault = rhs.mDefault;
		mValue = rhs.mValue;
	}
	CObj(const CObj<T>&& rhs)
	{	
                mType = move(rhs.mType);
                mMin = move(rhs.mMin);
                mMax = move(rhs.mMax);
                mDefault = move(rhs.mDefault);
                mValue = move(rhs.mValue);
	}
};

/* Bool is specialization: No Min|Max */
template <>
class CObj <bool>: public CObjBase
{
	bool mValue;
public:
	CObj(bool s=bool{}) : CObjBase(CObjType::BOOL) { mValue=s ;}
	void SetVal(bool s)
	{
		mValue=s;
	}
	bool GetVal()
	{
		return mValue;
	}
};
/* String  is also a specialization */
template <>
class CObj <string>: public CObjBase
{
	string mValue;
public:
	CObj(string s=string{}) : CObjBase(CObjType::STR) {mValue=s;}
	void SetVal(string s)
	{
		mValue=s;
	}
	string GetVal()
	{
		return mValue;
	}
};

/*
   A Command processing engine can have 2 modes
   a. Ascii mode
   b. Binary mode

(A) ASCII COMMANDS
  -  Traditionaly a  ASCII command is processed like this:
	user may have command such as :
	$Cmd Module SubModule Param Set xxxx
	in such case:
		we have to lookup Module->SubModule->Param->Set()
	these modules, submodules, params may be internally some enums
	so, they may be converted to ModuleEnum, SubModuleEnum, ParamEnum
	and then they may reside in different array, and we may hop differnt
	array index to reach actual object and then apply SET()|GET()
   -  In this implementation, this is how I will do it.
	Bootup time: user calls a binding of "Module SubModule Param" string to a UID.
	we lookup this UID in HashMap, then, get CObj assosiated with it. Apply SetVal!
(B) BINARY COMMANDS
  -  Traditionally:
	user jumps to different array to reach param, and then apply SET(),GET().
  -  In This Implementaiton:
	we GenerateUID based on ModuleID(it: max 255),SubModuleId(int:max x255) 
	and ParamID(short), and jump in HashMap to exact COjbBase and apply 
	SetVal|GetVal, and then call callback on the Value.
*/

typedef void*(*CallBack)(CObjBase* p);

class CCmd
{
	map<string,int> AsciiMap;
	map<int,CallBack> UIDMap;
	map<int,CObjBase*> Params;

	void  ProcessUIDCmd(int uid, string value)
	{ 
		if(UIDMap[uid]==nullptr) return;/*NO Callback*/
		CallBack cb = UIDMap[uid];
	  	CObjBase *p = Params[uid];
		switch(Params[uid]->GetType())
		{
			case  CObjType::I8: {
				char v = value.at(0);
				CObj<char> *o = reinterpret_cast<CObj<char> * > (p);
				o->SetVal(v);
			}
			case  CObjType::U8: {
				unsigned char v = value.at(0);
				CObj<unsigned char> *o = reinterpret_cast<CObj<unsigned char> * > (p);
				o->SetVal(v);
			}
			case  CObjType::I16: {
				short v = atoi(value.c_str());
				CObj<short> *o = reinterpret_cast<CObj<short> * > (p);
				o->SetVal(v);
			}
			case  CObjType::U16: {
				unsigned short v = atoi(value.c_str());
				CObj<unsigned short> *o = reinterpret_cast<CObj<unsigned short> * > (p);
				o->SetVal(v);
			}
			case  CObjType::I32: {
				int v = atoi(value.c_str());
				CObj<int> *o = reinterpret_cast<CObj<int> * > (p);
				o->SetVal(v);
			}
			case  CObjType::U32: {
				unsigned int v = atoi(value.c_str());
				CObj<unsigned int> *o = reinterpret_cast<CObj<unsigned int> * > (p);
				o->SetVal(v);
			}
			case  CObjType::STR: {
				CObj<string> *o = reinterpret_cast<CObj<string> * > (p);
				o->SetVal(value);
			}
			case  CObjType::F32: {
				float v = atof(value.c_str());
				CObj<float> *o = reinterpret_cast<CObj<float> * > (p);
				o->SetVal(v);
			}
			case  CObjType::BOOL: {
				bool v = (value.c_str()-'0')?true:false;
				CObj<bool> *o = reinterpret_cast<CObj<bool> * > (p);
				o->SetVal(v);
			}
			default: break;
		}//switch
		cb(p); // Call the CallBack!!
	}
public:
	int	GenerateUID(int  Mod, int SubMod, short Param)
	{
	return
		(
		(Mod & 0x000000ff)<<24 +
		(SubMod & 0x000000ff)<<16 +
		(Param)
		)
		;
	}
	string  GenerateToLowerStr(string s)
	{ 
		string k;
		s.erase(remove_if(s.begin(),s.end(),::isspace),s.end());
		transform(s.begin(), s.end(), k.begin(), ::tolower);
		return k;
		/* We strip all spaces, tabs, and make the string lower case, then store*/
		
	}
	void    RegisterParams (int uid, CObjBase* p)
	{
		Params[uid]=p;
	}

	void    RegisterCb (int uid, CallBack cb)
	{
		UIDMap[uid]=cb;
	}

	void    RegisterString(string s, int uid)
	{
		auto i = AsciiMap.find(s);
		if(i == AsciiMap.end()) AsciiMap.insert(pair<string,int>(s,uid));
		else i->second=uid;
	}
	void    SetValue(string s)
	{/*TODO: Split string to extract module, submodule and param name. */
	}

	void  SetValue(int Mod, int SubMod, short Param, string value)
	{
		int uid = GenerateUID(Mod,SubMod,Param);
		if(UIDMap.find(uid) != UIDMap.end()) {
			ProcessUIDCmd(uid, value);
		}
	}
	CObjBase * GetValue(string s)
	{
		CObjBase *p = nullptr;
		string k = GenerateToLowerStr(s);
		auto i = AsciiMap.find(k);
		if(i!= AsciiMap.end()) {
			int uid = i->second;
			if(UIDMap.find(uid) != UIDMap.end())
                	{
                       		p = Params[uid];
                	}
		}
		return p;
	}
	CObjBase *GetValue(int Mod, int SubMod, short Param)
	{
		CObjBase *p = nullptr;
		int uid = GenerateUID(Mod,SubMod,Param);
		if(UIDMap.find(uid) != UIDMap.end()) {
			p =  Params[uid];
		}
		return p;
	}
	CCmd(){}
	~CCmd()
	{ 
		AsciiMap.clear();
		UIDMap.clear();
		for(auto i: Params) delete(i.second);
		Params.clear();
	}
};
#endif// _COBJ_CLASS_H_
