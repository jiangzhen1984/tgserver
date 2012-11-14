


#include "demo1.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "header.h"

#define LINE_FILE "/tmp/line"
#define POINT_FILE "/tmp/point"
#define TAG "DEMO1"


static ShmHeader * g_header=NULL;


static void initPoint()
{
	PPoint  * pHeader=g_header->getPHeader();
	if(pHeader!=NULL)
	{
		int i=0;
		for(i=0;i<g_header->getPC();i++)
		{
			PPoint tmp((i+1)*15,(i+1)*15);
			memcpy(pHeader+i,&tmp,sizeof(class PPoint));
			printf("======address:%d   x:%d,y:%d\n",pHeader+i,(pHeader+i)->getX(),(pHeader+i)->getY());
		}
		
		munmap(pHeader,sizeof(class PPoint)*10);
	}
}

static void initLine()
{
	PLine * pHeader=g_header->getLHeader();
	if(pHeader!=NULL)
	{
		int i=0;
		for(i=0;i<g_header->getLC();i++)
		{
			PLine line((i+1) *5 , (i+1) *8, (i+1) *25,(i+1) *25);
			memcpy(pHeader+i,&line,sizeof(class PLine));
		}
		munmap(pHeader,sizeof(class PLine)*10);
	}
}


void * create(char * pPar)
{
	g_header=new ShmHeader;
	
	setenv("LOG_LEVEL","3",1);

	int fd=open(POINT_FILE,O_CREAT|O_RDWR,0777);
	if(fd<0)
	{
		printf(" can not create shared memory:%s  errno:%d\n",POINT_FILE,errno);	
	}
	else
	{
		lseek(fd,sizeof(class PPoint)*10,SEEK_SET);	
		write(fd,"",1);
		printf(" create shared memory\n");

		PPoint * pPoints=(PPoint *)mmap(NULL,sizeof(class PPoint)*10,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
		printf("======address:%d---init\n",pPoints);
		close(fd);
		if(pPoints!=NULL)
		{
			g_header->setPointHeader(pPoints);
			g_header->setPointsCount(10);
		}
		else
		{
			printf(" can not mmap point\n");
		}
	}


	fd=open(LINE_FILE,O_CREAT|O_RDWR,0777);
	if(fd<0)
	{
		printf(" can not create shared memory:%s\n",LINE_FILE);	
	}
	else
	{
		lseek(fd,sizeof(class PLine)*10,SEEK_SET);	
		write(fd,"",1);

		PLine * pLines=(PLine *)mmap(NULL,sizeof(class PPoint)*10,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0); if(pLines!=NULL) { g_header->setLineHeader(pLines);
			g_header->setLinesCount(10);
		close(fd);
		}
		else
		{
			printf(" can not mmap line\n");
		}
	}


	initPoint();
	initLine();

	return NULL;
}


int init(void * pPar)
{
}


void * do_request(void * pPar)
{
	char * req=(char *) pPar;
	printf("===get:%s\n",(char *)pPar);
	// get all data 
	if(strcmp(req,"tg://libdemo1.so/all")==0)
	{
		int allSize=g_header->getPC()* sizeof(class PPoint)+ 4+4;

		char * pData=(char * )malloc(allSize);
		memset(pData,0,allSize);
		memcpy(pData,&allSize,4);
		int pCount=g_header->getPC();
		memcpy(&pData[4],&pCount,4);

		int fd=open(POINT_FILE,O_CREAT|O_RDWR,0777);
		if(fd<0)
		{
			return NULL;
		}
		PPoint * pPoints=(PPoint *)mmap(NULL,sizeof(class PPoint)*10,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

		PPoint * pHeader=g_header->getPHeader();
		int i=0;
		for(i=0; i<g_header->getPC();i++)
		{
			memcpy(&pData[i*sizeof(class PPoint) +8],pHeader+i,sizeof(struct PPoint));
		}

		munmap(pHeader,sizeof(class PLine)*10);

		printf(" returned data:");	
		//logM(pData,allSize);

		int size=(unsigned char)pData[3] >> 24 | (unsigned char )pData[2] >> 16 | (unsigned char )pData[1] >> 8 | (unsigned char )pData[0];
		printf("=====%d\n",size);
		
		return pData;	
	}
}


int release(void * pPar)
{
}


void destroy(void * pPar)
{
	if(g_header!=NULL)
	{
		memset(g_header,0,sizeof(class ShmHeader));
		delete g_header;
		g_header=NULL;
	}
}

