

#include "tg_service.h"

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef LINUX
#include <dlfcn.h>
#elif WIN32
#endif
#include <string.h>
#include "tg_log.h"
#include "tg_util.h"


#define TAG ("SERVICE")


struct TGService ** g_services=NULL;
int g_services_counts=0;



static int appendGServiceArrayCount()
{

	int size=(g_services_counts+DEFAULT_SERVICE_SIZE)*sizeof(struct TGService *);
	struct TGService ** _ppservices=(struct TGService **)malloc(size);
	logI(TAG," malloc service array from %d to %d", (long) & _ppservices[0],(long)&_ppservices[g_services_counts+DEFAULT_SERVICE_SIZE]);
	if(_ppservices==NULL)
	{
		return -1;
	}

	if(g_services==NULL)
	{
		logI(TAG," initilize service array space");
		g_services=_ppservices;
	}
	else
	{
		memcpy(_ppservices,g_services,g_services_counts*sizeof(struct TGService *));
		free(g_services);
		g_services=_ppservices;
	}

	g_services_counts+=DEFAULT_SERVICE_SIZE;

	return 0;
}


static void addService(struct TGService * pSer)
{
	if(pSer==NULL)
	{
		return;
	}
	//FIXME linked list or array?
	if(g_services==NULL)
	{
		logI(TAG," create new space for service array");
		appendGServiceArrayCount();
		g_services[0]=pSer;
		return;
	}
	else
	{
		int i=0;
		for(i=0;i<g_services_counts;i++)
		{
			struct TGService * pS=g_services[i];
			if(pS==NULL)
			{
				g_services[i]=pSer;
				logI(TAG,"load library: id:%d name:%s",pSer->sID,pSer->serviceName);
				return;
			}
		}
		
		//if still not return, means the array is full.
		// we need append space,
		if(appendGServiceArrayCount()==0)
		{
			g_services[i]=pSer;
			logI(TAG,"load library: id:%d name:%s",pSer->sID,pSer->serviceName);
		}
		else
		{
			logE(TAG,"can not push service in to array. cause by: malloc space error");
		}
	}
}



// check library 
// check some functions  exist in library or not
// if it doesn't exist in library return -1 
// else if it's correct return 0 and fill functions address into ppService
static int checkLib(void * dlHandler,struct TGService ** ppService)
{
	static int sID=0;
	struct TGService * pService=(struct TGService *) malloc(sizeof(struct TGService));
	memset(pService,0,sizeof(struct TGService));

	SERVICE_CREATE_FUNCTION fpC=NULL;
	SERVICE_INIT_FUNCTION fpI=NULL;
	SERVICE_DO_REQUEST_FUNCTION fpDo=NULL;
	SERVICE_RELEASE_FUNCTION fpR=NULL;
	SERVICE_DESTROY_FUNCTION fpD=NULL;
	
#ifdef LINUX
	fpC=dlsym(dlHandler,SERVICE_CREATE_FUNCTION_NAME);
	fpI=dlsym(dlHandler,SERVICE_INIT_FUNCTION_NAME);
	fpDo=dlsym(dlHandler,SERVICE_DO_REQUEST_FUNCTION_NAME);
	fpR=dlsym(dlHandler,SERVICE_RELEASE_FUNCTION_NAME);
	fpD=dlsym(dlHandler,SERVICE_DESTROY_FUNCTION_NAME);
#elif WIN32
#endif

	int flag;
	if(fpC!=NULL && fpI!=NULL && fpDo!=NULL && fpR!=NULL && fpD!=NULL)
	{
		pService->sID=sID++;	
		pService->do_create=fpC;
		pService->do_init=fpI;
		pService->do_request=fpDo;
		pService->do_release=fpR;
		pService->do_destroy=fpD;
		flag=0;
		*ppService=pService;
		logI(TAG," checked successfully flag:%d create:%s init:%s request:%s release:%s desotry:%s",flag,(long)&fpC,(long)&fpI,(long)&fpDo,(long)&fpR,(long)&fpD);
	}
	else
	{
		flag=-1;
		logE(TAG," checked library error:%s %s %s %s %s",fpC,fpI,fpDo,fpR,fpD);
		free(pService);
	}
	

	return flag;
}

void loadService(const char * path)
{
	if(path==NULL || strlen(path)==0)
	{
		logN(TAG ,LOG_LEVEL_WARNING," path is empty");
		return;
	}

	logI(TAG," prepare to load service from library:%s",path);
	
	void * dlHandler=NULL;
	char * errmsg;
#ifdef LINUX
	dlHandler=dlopen(path,RTLD_NOW);
	if(dlHandler==NULL)
	{
		errmsg=dlerror();
		logN("SERVICE",LOG_LEVEL_WARNING," can not open library:%s %s",path,errmsg);
		return;
	}
#elif WIN32
#endif
	
	struct TGService * pService=NULL;
	if(checkLib(dlHandler,&pService)==0)
	{
		logD(TAG,"check lib successfully");
		//FIXME use system seportor
		char * pLibName=NULL;
		getLibName(path,&pLibName);
		if(pLibName==NULL)
		{
			pLibName=(char *)malloc(sizeof(5));	
			memset(pLibName,0,10);
			sprintf(pLibName,"%d",pService->sID);
		}
		
		pService->serviceName=pLibName;
		addService(pService);
	}
	else
	{
		logW(TAG,"check library error");
	}

}





//unload all services from global list
void unLoadService()
{
	if(g_services==NULL)
	{
		return;
	}
	struct TGService **ppHead=g_services;
	while(*ppHead!=NULL)
	{
		(*ppHead)->parCounts=0;

		if((*ppHead)->serviceName!=NULL)
		{
			free((*ppHead)->serviceName);
			(*ppHead)->serviceName=NULL;
		}
		char ** ppara=(*ppHead)->parameters;
		while(ppara!=NULL && *ppara!=NULL)
		{
			free(*ppara++);
		}
		(*ppHead)->parameters=NULL;

		ppHead++;
	}
	memset(g_services,0,g_services_counts*sizeof(struct TGService * ));
	free(g_services);
	g_services=NULL;
	g_services_counts=0;
}



static void copyService(struct TGService * pSource,struct TGService **ppDes)
{
	if(pSource == NULL)
	{
		logW(TAG,"didn't copy service cause by source empty! ");
		return;
	}
	
	struct TGService * pService=(struct TGService *)malloc(sizeof(struct TGService));
	memset(pService,0,sizeof(struct TGService));
	memcpy(pService,pSource,sizeof(struct TGService));
	// copy name 
	if(pSource->serviceName!=NULL)
	{
		int nameSize=strlen(pSource->serviceName);
		char * buf=(char *)malloc(nameSize);
		memset(buf,0,nameSize);
		memcpy(buf,pSource->serviceName,nameSize);
		pService->serviceName=buf;
	}
	else
	{
		pService->serviceName=NULL;
	}

	
	*ppDes=pService;
}


// find struct TGService according to id
// and return new TGService and return NULL if not found
// Note: The returnd service must be free after use
struct TGService * findServiceI(const int id)
{
	struct TGService * pService=NULL;
	if(g_services!=NULL)
	{
		struct TGService ** ppHead=g_services;
		struct TGService * pS=NULL;
		while((pS=*ppHead++)!=NULL)
		{
			if(pS->sID==id)
			{
				copyService(pS,&pService);
				//pService=pS;
				break;
			}
		}
	}
	
	return pService;

	
}


// find struct TGService according to id
// and return new TGService and return NULL if not found
// Note: The returnd service must be free after use
struct TGService * findServiceN(const char * name)
{
	struct TGService * pService=NULL;
	if(g_services!=NULL)
	{
		struct TGService ** ppHead=g_services;
		struct TGService * pS=NULL;
		while((pS=*ppHead++)!=NULL && pS->serviceName!=NULL)
		{
			if(strcmp(pS->serviceName,name)==0)
			{
				copyService(pS,&pService);
				//pService=pS;
				break;
			}	
		}
	}
	
	return pService;

}


void setServiceCreateParametersByN(const char * name,const char * para)
{
	if(name==NULL || *name=='\0')
	{
		return;
	}

	if(g_services!=NULL)
	{
		struct TGService ** ppHead=g_services;
		struct TGService * pS=NULL;
		while((pS=*ppHead++)!=NULL && pS->serviceName!=NULL)
		{
			
			if(strcmp(pS->serviceName,name)==0)
			{
				setServiceCreateParameters(pS,para);
				return;
			}
		}
	}
	logW(TAG," can not set parameters for service cause by service not found:%s",name);
}


// copy parameters to pSer
void setServiceCreateParameters(struct TGService * pSer,const char * para)
{
	if(pSer==NULL)
	{
		logE(TAG," service not exist");
		return;
	}

	if(pSer->parameters==NULL)
	{
		pSer->parCounts=0;
		int size=SERVICE_DEFAULT_PARA_COUNTS*sizeof(char *);
		pSer->parameters=(char **)malloc(size);
		memset(pSer->parameters,0,size);
	}
	// parameters is out of array need create new 
	else if(pSer->parCounts!=0 && (pSer->parCounts)%SERVICE_DEFAULT_PARA_COUNTS==0)
	{
		int size=(pSer->parCounts+SERVICE_DEFAULT_PARA_COUNTS)*sizeof(char *);
		logI(TAG," create new parameters for id:%d parCounts:%d", pSer->sID,pSer->parCounts);
		char ** pOld=pSer->parameters;
		pSer->parameters=(char **)malloc(size);
		memset(pSer->parameters,0,size);
		memcpy(pSer->parameters,pOld,pSer->parCounts*sizeof(char *));
		free(pOld);
	}

	char *pBuf=NULL;
	int bufSize=strlen(para)+1;
	pBuf=(char *)malloc(bufSize);
	memset(pBuf,0,bufSize);
	strncpy(pBuf,para,bufSize-1);
	pBuf[bufSize-1]='\0';
	pSer->parameters[pSer->parCounts++]=pBuf;
	logD(TAG,"  parameters:%d,  %s",pSer->parCounts,pSer->parameters);
	
}


// when server is starting up 
// server call this function for initialize all service 
void createAllServices()
{
	int count=0;
	if(g_services!=NULL)
	{	
		struct TGService ** ppHead=g_services;
		struct TGService * pS=NULL;
		while((pS=*ppHead++)!=NULL)
		{
			logI(TAG," start to creat service:%s",pS->serviceName);
			void * pData=pS->do_create(pS->parameters);	
			pS->pGlobalData=pData;
			logI(TAG," finish creating service:%s",pS->serviceName);
			count++;
		}
	}
}


// when client connection is established
// this function is called by client process
// Note: this function never is called in main process
void initAllServices()
{
	if(g_services!=NULL)
	{
		struct TGService ** ppHead=g_services;
		struct TGService * pS=NULL;
		while((pS=*ppHead++)!=NULL)
		{
			logI(TAG," start to initialize service:%s parameters:%s",pS->serviceName,pS->parameters);
			pS->do_init(pS->parameters);
			logI(TAG," finish initializeing service:%s",pS->serviceName);
		}
	}
}

// when client connection is closed
// this function is called by client process
// Note: this function never is called in main process
void releaseAllServices()
{
	if(g_services!=NULL)
	{
		struct TGService ** ppHead=g_services;
		struct TGService * pS=NULL;
		while((pS=*ppHead++)!=NULL)
		{
			logI(TAG," start to release service:%s",pS->serviceName);
			pS->do_release(pS->pGlobalData);	
			logI(TAG," finish releaseing service:%s",pS->serviceName);
		}
	}
}

// when server is shuting down
// server call this function for destroing all service
void destroyAllServices()
{
	if(g_services!=NULL)
	{
		struct TGService ** ppHead=g_services;
		struct TGService * pS=NULL;
		while((pS=*ppHead++)!=NULL)
		{
			pS->do_destroy(pS->parameters);	
		}
	}
}

