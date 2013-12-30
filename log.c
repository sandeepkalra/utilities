#include "log.h"


Writer CLog::w = Writer::GetInstance();
const char *program = "log.c";
const char * module = "main";
int main()
{
	setlogmask (LOG_UPTO (LOG_DEBUG)); // you can mute everything from here if you want!
	CLog Test(program, module);
	Test.SetLevel(ELogLevel::ERROR);//want to listen only this and above
	Test.E("Hi! %d",10);
	Test.D("Hi! %s, %d", "debug logs here ", 10);
	Test.I("Hi! %p, %s", &Test, "info");
	Test.C("Hi! %s", "critical error");
}
