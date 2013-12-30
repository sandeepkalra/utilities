#ifndef _LOGBASE_H_
#define _LOGBASE_H_

#include <iostream>
#include <string.h>
#include <syslog.h>
#include <stdarg.h>
using namespace std;
#define MAX_LENGTH (512)

namespace {
 class Writer
 {
	char _buff[MAX_LENGTH];
	Writer() = default;
	bool opened=false;
public:
	static const Writer& GetInstance()
	{
	   static Writer w;
	   return w;
	}
	void Start(const char *p)
	{ 
		if(!opened){
			openlog(p,LOG_PID|LOG_CONS|LOG_NDELAY,LOG_LOCAL1);
			opened=true;
			}
	}
	~Writer()
	{
		if(opened) 
		{ 
			closelog();
			opened = false;
		}
	}
	void Write(int priority, const char *frmt, va_list args) 
	{ 
		if(opened) 
		{
			vsnprintf(_buff,MAX_LENGTH, frmt , args);
			syslog(priority, _buff, 0); 
		}
	}
 } ;
}//hide me from the world!



#endif // _LOGBASE_H_
