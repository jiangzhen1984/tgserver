
#ifndef TG_LOG_H
#define TG_LOG_H

#define LOG_LEVEL_ERROR (0)
#define LOG_LEVEL_WARNING (1)
#define LOG_LEVEL_INFO (2)
#define LOG_LEVEL_DEBUG (3)

#ifdef LINUX
#include <errno.h>
#define GET_ERR_MSG(msg) msg=strerror(errno);
#elif WIN32
#define GET_ERR_MSG(msg) msg="";
#endif

// origin log function
void logN(const char * tag,const int level,const char * fmt,...);

// print message of debug level
void logD(const char * tag,const char * fmt,...);

// print message of warning level
void logW(const char * tag,const char * fmt,...);

// print message of INFO level
void logI(const char * tag,const char * fmt,...);

// print message of error level
void logE(const char * tag,const char * fmt,...);

// used to print memory
void logM(const char * pData, const int size);

#endif
