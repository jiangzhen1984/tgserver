

#include "tg_util.h"
#include <stdlib.h>
#include <string.h>
#include "tg_log.h"


int findSeparator(char * source,char * seopr)
{
	int pos=-1;
	if(source !=NULL && seopr!=NULL)
	{
		int sLen=strlen(seopr);
		int fLen=strlen(source);
		if(sLen>fLen)
		{
			return pos;
		}
		char c[fLen+1];
		memcpy(c,source,fLen);
		c[fLen]='\0';

		int findSize=fLen-sLen;
		while(++pos<=findSize)
		{
			char  pt=c[pos];
			if(strncmp(&pt,seopr,sLen)==0)
			{
				return pos;
			}
		}
		// not found
		pos=-1;
		
	}
	return pos;
}

int lastFindSeparator(char * source,char * seopr)
{
	int pos=-1;
	if(source !=NULL && seopr!=NULL)
	{
		int sLen=strlen(seopr);
		int fLen=strlen(source);
		if(sLen>fLen)
		{
			return pos;
		}
		char c[fLen+1];
		memcpy(c,source,fLen);
		c[fLen]='\0';

		pos=fLen-sLen+1;
		while(pos-->=0)
		{
			char  pt=c[pos];
			if(strncmp(&pt,seopr,sLen)==0)
			{
				return pos;
			}
		}
	}
	return pos;
}


void getLibName(char * path,char ** ppName)
{
	if(path!=NULL)
	{
		int size=strlen(path)+1;
		char buf[size];
		memset(buf,0,size);
		memcpy(buf,path,size-1);
		buf[size-1]='\0';
		//FIXME should use maco of system
		int pos=lastFindSeparator(buf,"/");	
		if(pos>=0)
		{
			int nsize=size-pos;	
			char * pc=(char *) malloc(nsize);
			memset(pc,0,nsize);
			strncpy(pc,&buf[pos+1],nsize-1);
			*ppName=pc;
			logD("UTIL"," get lib name:%s\n",pc);
		}
	}
	
}


int getTGServerPid()
{
#ifdef LINUX
	return getpid();
#elif WIN32
	return -1;
#endif

}

