
#ifndef DEMO_H
#define DEMO_H
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include "../../tg_log.h"

#define  LINE_KEY "line"
#define  POINT_KEY "point"

static void * pShmLineHead=NULL;
static int lineShmID=-1;

static void * pShmPointHead=NULL;
static int pointShmId=-1;


struct Head{
	void * pLineShmHead;
	void * pPointShmHead;
};


struct point{
	int x;
	int y;
};

struct line{
	int x;
	int y;
	int endx;
	int endy;
};

void * create(char * pPar)
{
	printf("##################### create\n");
	struct shmid_ds buf;
	lineShmID=shmget(ftok(LINE_KEY,0),102400,IPC_CREAT|0666); 
	printf(" create share memory line:%d\n",lineShmID);	
	if(lineShmID!=-1) { 
		pShmLineHead=shmat(lineShmID,NULL,0); shmctl(lineShmID,IPC_STAT,&buf);
		shmctl(lineShmID,IPC_STAT,&buf);
	}

	// init point array shared memory
	struct shmid_ds buf1;	
	pointShmId=shmget(ftok(POINT_KEY,10),102400,IPC_CREAT|0666);
	printf(" create share memory point:%d\n",pointShmId);	
	if(pointShmId!=-1)
	{
		pShmPointHead=shmat(pointShmId,NULL,0);
		shmctl(pointShmId,IPC_STAT,&buf1);
	}

	struct Head * pHead=(struct Head* ) malloc(sizeof(struct Head));	
	memset(pHead,0,sizeof(struct Head));
	pHead->pLineShmHead=pShmLineHead;
	pHead->pPointShmHead=pShmPointHead;
	
	printf("##################### create finish\n");
	return pHead;
}


//|---reserved 10 bytes--|  length of 4 bytes--|--- data segment---|
static void initPoint()
{

	printf("##################### init :%d\n",pShmPointHead);
	int i=0;
	struct point * p=(struct point *)&pShmPointHead[14];
	for(i=0;i<18;i++)
	{
		p->x=(i+1)*15;
		p->y=(i+1)*15;
		p++;
	}
	memcpy(&pShmPointHead[10],&i,4);
}

//|---reserved 10 bytes--|  length of 4 bytes--|--- data segment---|
static void initLine()
{
	int i=0;
	struct line* p=(struct line*)&pShmLineHead[14];
	for(i=0;i<19;i++)
	{
		p->x=(i+1)*15;
		p->y=(i+1)*15;
		p->endx=(i+1)*25;
		p->endy=(i+1)*25;
		p++;
	}
	
	memcpy(&pShmLineHead[10],&i,4);
}

int init(void * pPar)
{
	initPoint();
//	initLine();
	return 0;
}


void * do_request(void * pPar)
{
	char * req=(char *) pPar;
	printf("===get:%s\n",(char *)pPar);
	// get all data 
	if(strcmp(req,"tg://libdemo.so/all")==0)
	{
		char * pPointSize=malloc(4);
		memset(pPointSize,0,4);
		memcpy(pPointSize,&pShmPointHead[10],4);
		int pointSize=pPointSize[3]  >>24 | pPointSize[2]  >>16 | pPointSize[1] >>8 | pPointSize[0] ;

		unsigned int allSize=0;
		allSize=pointSize+pointSize*sizeof(struct point); //+lineSize*sizeof(struct line)+4+4;

		char * pData=(char * )malloc(allSize+4);
		memset(pData,0,allSize+4);
		printf("---point size:%d  all size:%d\n",pointSize,allSize);

		memcpy(pData,&allSize,4);

		// copy point data lenth and data
		memcpy(&pData[4],&pointSize,4);
		memcpy(&pData[8],&pShmPointHead[14],pointSize*sizeof(struct point));

		printf(" returned data:");	
		logM(pData,allSize+4);
		printf(" shared memory  data:");	
		logM(&pShmPointHead[14],allSize);

		int size=(unsigned char)pData[3] >> 24 | (unsigned char )pData[2] >> 16 | (unsigned char )pData[1] >> 8 | (unsigned char )pData[0];
		printf("=====%d\n",size);
		
		return pData;	
	}
}
			
		


int release(void * pPar)
{
	shmdt(pShmLineHead);
	shmdt(pShmPointHead);
	return 0;
}


void destroy(void * pPar)
{
	shmctl(lineShmID,IPC_RMID,NULL);
	shmctl(pointShmId,IPC_RMID,NULL);
}

#endif
