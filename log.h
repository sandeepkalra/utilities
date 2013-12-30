#ifndef _CLOG_H_
#define  _CLOG_H_
#include "logbase.h"

enum class ELogLevel {
	DEBUG,
	INFO,
	ERROR,
	CRITICAL,
};

class CLog
{
	char _buff[MAX_LENGTH];
	int module_strlen = 0 ;
	int LogTypeOffset = 0; 
	int LogStartPoint = 0 ;
	static Writer w ;
	// make sure to call "Writer CLog::w = Writer::GetInstance();" in CPP
	ELogLevel lvl = ELogLevel::ERROR;
	
public:
	CLog(const char *program, const char *module)
	{
		module_strlen = strlen(module);
		strncpy(_buff, module, module_strlen);
		strncpy(_buff+module_strlen,"_m: ",strlen("_m: "));
		LogTypeOffset = module_strlen + 1;
		LogStartPoint = module_strlen + strlen("_m: ");
		w.Start(program);
	}
	void SetLevel(ELogLevel l) { lvl = l; }
	void D(const char *p,...){
		if(lvl > ELogLevel::DEBUG) return;
		_buff[LogTypeOffset] = 'D';
		strcpy(_buff + LogStartPoint, p);
		va_list valist; 
		va_start(valist, p);
		w.Write(LOG_DEBUG, _buff, valist);
		va_end(valist);
	}
	void I(const char *p,...){
		if(lvl > ELogLevel::INFO) return;
		_buff[LogTypeOffset] = 'I';
		strcpy(_buff + LogStartPoint, p);
		va_list valist; 
		va_start(valist, p);
		w.Write(LOG_INFO, _buff, valist);
		va_end(valist);
	}
	void E(const char *p,...){
		if(lvl > ELogLevel::ERROR) return;
		_buff[LogTypeOffset] = 'E';
		strcpy(_buff + LogStartPoint, p);
		va_list valist; 
		va_start(valist, p);
		w.Write(LOG_ERR, _buff, valist);
		va_end(valist);
	}
	void C(const char *p,...){
		_buff[LogTypeOffset] = 'C';
		strcpy(_buff + LogStartPoint, p);
		va_list valist; 
		va_start(valist, p);
		w.Write(LOG_EMERG, _buff, valist);
		va_end(valist);
	}
};
#endif //  _CLOG_H_
