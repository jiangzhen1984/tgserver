

#include "tg_log.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#define BUFFER_SIZE 1024



// native print message function 
// just wrap printf
static void __log(const char * tag, const int level,char * msg)
{

	char * czLevel=getenv("LOG_LEVEL");
	int _l=czLevel==NULL?LOG_LEVEL_WARNING:atoi(czLevel);
	time_t ticks;
	ticks=time(NULL);
	
	char * szPrintLogLevel;
	switch(level)
	{
		case LOG_LEVEL_ERROR:
			szPrintLogLevel=" [ERROR] ";
			break;
		case LOG_LEVEL_WARNING:
			szPrintLogLevel=" [WARNING] ";
			break;
		case LOG_LEVEL_INFO:
			szPrintLogLevel=" [INFO] ";
			break;
		case LOG_LEVEL_DEBUG:
			szPrintLogLevel=" [DEBUG] ";
			break;
		default:
			szPrintLogLevel=" [INFO] ";
			
	}

	if(_l>=level)
	{
		char buf_head[100];
		snprintf(buf_head,sizeof(buf_head),"%.24s[%s]%s==>",ctime(&ticks),tag,szPrintLogLevel);
		printf("%s",buf_head);
		printf("%s\n",msg);
	}

}
	

void logN(const char * tag,const int level,const char * fmt,...)
{
	
	char buf[BUFFER_SIZE];
	va_list ap;
	va_start(ap,fmt);
	vsnprintf(buf,BUFFER_SIZE,fmt,ap);
	va_end(ap);
	__log(tag,level,buf);
}




// print message of debug level
void logD(const char * tag,const char * fmt,...)
{
	char buf[BUFFER_SIZE];
	va_list ap;
	va_start(ap,fmt);
	vsnprintf(buf,BUFFER_SIZE,fmt,ap);
	va_end(ap);
	__log(tag,LOG_LEVEL_DEBUG,buf);
}

// print message of warning level
void logW(const char * tag,const char * fmt,...)
{
	char buf[BUFFER_SIZE];
	va_list ap;
	va_start(ap,fmt);
	vsnprintf(buf,BUFFER_SIZE,fmt,ap);
	va_end(ap);
	__log(tag,LOG_LEVEL_WARNING,buf);
}

// print message of INFO level
void logI(const char * tag,const char * fmt,...)
{

	char buf[BUFFER_SIZE];
	va_list ap;
	va_start(ap,fmt);
	vsnprintf(buf,BUFFER_SIZE,fmt,ap);
	va_end(ap);
	__log(tag,LOG_LEVEL_INFO,buf);
}

// print message of error level
void logE(const char * tag,const char * fmt,...)
{

	char buf[BUFFER_SIZE];
	va_list ap;
	va_start(ap,fmt);
	vsnprintf(buf,BUFFER_SIZE,fmt,ap);
	va_end(ap);
	__log(tag,LOG_LEVEL_ERROR,buf);
}



void logM(const char * pData, const int size)
{
	int i=0;
	for(i=0;i<size;i++)
	{
		printf("%02x",(unsigned char)pData[i]);
		if(i%4==0)
		{
			printf(" ");
		}
		
	}
	printf("\n");
}


