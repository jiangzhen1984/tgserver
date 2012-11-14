
#include "tg_server.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tg_context.h"
#include "tg_service.h"
#include "tg_log.h"
#include "tg_util.h"


#define TAG "SERVER"



#ifdef LINUX
#include <dirent.h>
#include <errno.h>
#define SEPATOR ".so"
#elif WIN32
#define SEPATOR ".dll"
#endif

static void __load_services(char * dir)
{
	if(dir==NULL || *dir=='\0')
	{
		logE(TAG,"load service error: dir is empty");
		return;
	}

#ifdef LINUX
	DIR * pDir=NULL;
	pDir=opendir(dir);
	if(pDir==NULL)
	{
		logE(TAG," directory not found:%s",dir);
		return;
	}
	struct dirent * pEntry=NULL;
	while((pEntry=readdir(pDir))!=NULL)
	{
		logD(TAG," read file:%s  type:%d",pEntry->d_name,pEntry->d_type);
		if(pEntry->d_type==DT_REG)
		{
			char path[strlen(dir)+strlen(pEntry->d_name)+2];
			memset(path,0,sizeof(path));
			strncpy(path,dir,strlen(dir));
			strcat(path,"/");
			strcat(path,pEntry->d_name);
			logD(TAG," read lib:%s  ",path);
			loadService(path);
		
		}
	}

#elif WIN32
#endif
}



static void __parse_parameter(char * pPar,char * pVal)
{
	int port,size,maxClients;
	switch(*pPar)
	{
		//get port value
		case 'p':
			port=atoi(pVal);
			g_context->pServer->port=(port==0?DEFAULT_LISTEN_PORT:port);
			break;
		case 'l':
			size=strlen(pVal)+1;
			char *  pBuf=(char *)malloc(size);
			memset(pBuf,0,size);
			memcpy(pBuf,pVal,size-1);
			g_context->pServer->listen_ip=pBuf;
			break;
		case 'm':
			maxClients=atoi(pVal);
			g_context->pServer->maxClients=maxClients==0?DEFAULT_MAX_CLIENTS:maxClients;
			break;
		case 'o':
			switch(*pVal)
			{
				case '0':
					g_context->logLevel=LOG_LEVEL_ERROR;
					setenv("LOG_LEVEL","0",1);
					break;
				case '1':
					g_context->logLevel=LOG_LEVEL_WARNING;
					setenv("LOG_LEVEL","1",1);
					break;
				case '2':
					g_context->logLevel=LOG_LEVEL_INFO;
					setenv("LOG_LEVEL","2",1);
					break;
				case '3':
					g_context->logLevel=LOG_LEVEL_DEBUG;
					setenv("LOG_LEVEL","3",1);
					break;
				default:
					g_context->logLevel=LOG_LEVEL_ERROR;
					setenv("LOG_LEVEL","0",1);
					break;
			}
			break;
		default:
			logW(TAG," unknow parameter:%s value:%s",pPar,pVal);
	}

}

// read context configuration files
static void __read_conf_file(char * filePath)
{
	if(filePath==NULL || *filePath=='\0')
	{
		logE(TAG," file not found");
		return;
	}
	FILE * fp;
	fp=fopen(filePath,"r");
	if(fp<=0)
	{
		char * msg=NULL;
#ifdef LINUX
		msg=strerror(errno);
#elif WIN32
#endif
		logE(TAG,"read conf error:%s  errmsg:%s",filePath,msg);
		return;
	}
	char line[500];
	int bSize=sizeof(line);
	memset(line,0,bSize);
	while(fgets(line,bSize,fp)!=NULL)
	{
		if(line[0]=='\0' || line[0]=='\n')
		{
			continue;
		}
		logD(TAG,"read conf line:%s",line);
		int separPos=findSeparator(line,"=");
		if(separPos==-1)
		{
			logE(TAG," conf context error:%s",line);
			continue;
		}
		char pPar[separPos+1];
		memset(pPar,0,separPos+1);
		memcpy(pPar,line,separPos);

		//get value of parameter
		int valSize=strlen(line)-separPos;
		char pVal[valSize];
		memset(pVal,0,valSize);
		memcpy(pVal,&line[separPos+1],valSize-1);
		if(pPar[0]!='[')
		{
			char * fpar=NULL;
			if(strncmp(pPar,"port",4)==0)
			{
				fpar="p";
			}
			else if(strncmp(pPar,"listen",6)==0)
			{
				fpar="l";
			}
			else if(strncmp(pPar,"max-clients",11)==0)
			{
				fpar="m";
			}
			else if(strncmp(pPar,"log-level",9)==0)
			{
				fpar="o";
			}
			__parse_parameter(fpar,pVal);
		}
		else
		{
			if(strncmp(pPar,"[D]",3)==0 && strlen(pPar)>3)
			{
				logD(TAG," set service parameter:%s",&pPar[3]);
				setServiceCreateParametersByN(&pPar[3],pVal);
			}
		}
		
	}
	fclose(fp);
	logI(TAG," finished read conf file:%s",filePath);
} 

// init server context according to stratup parameters
// -p port -l listen-path -m max-clients -s service dir or path -s service dir 
// -o loglevel(0-ERROR 1-WARNING 2-INFO 3-DEBUG)
// -D servicename(library name)=service paramers
// -f parameters-file-path the file rule is following:
//	port=	
//	listen=
//	max-clients=
//	[D]service-name=paramters
// Note: the service paramters will pass to create and init functions
// return 0 parse successfully
// return -1 parse error
int init_context(int argc, char** argv)
{
	int flag=0;
	// init context object
	init();

	char dirBuf[255];
	memset(dirBuf,0,sizeof(dirBuf));
	char * pDir=getenv("TGSERVER_HOME");
	if(pDir!=NULL)
	{
		memcpy(dirBuf,pDir,strlen(pDir));
	}
	else
	{
		memcpy(dirBuf,"../",4);
	}

	// init plugin lib
	{
	char sDir[255];
	memset(sDir,0,sizeof(sDir));
	strcat(sDir,dirBuf);
	strcat(sDir,DEFAULT_SERVICE_DIR);
	__load_services(sDir);
	}
	
	if(argc==1)
	{
		char file[255];
		memset(file,0,sizeof(file));
		strcat(file,dirBuf);
		strcat(file,DEFAULT_CONF_PATH);
		__read_conf_file(file);
		flag=0;
	}

	while(argv!=NULL && *++argv!=NULL && **argv!='\0') {

		char * pPar=*argv;	
		char * pVal=*++argv;
		logI(TAG,"paramters count:%d get parameter:%s %s",argc,pPar,pVal);
		if(pPar == NULL || *pPar != '-' || *++pPar=='\0' || pVal==NULL || *pVal=='\0' )
		{
			logW(TAG," parse parameter error:par:%s  val:%s", pPar, pVal);
			flag=-1;
			// release all resource
			release();
			break;
		}
		
		
		switch(*pPar)
		{
			case 'f':
				__read_conf_file(pVal);
				break;
			case 's':
				__load_services(pVal);
				break;
			// this parameters is service parameters
			case 'D':
				break;
			default:
				__parse_parameter(pPar,pVal);
				
		}
	}

	logI(TAG," init context result:%d",flag);

#ifdef DEBUG
	printf("#######################\n");
#endif
	return flag;
}


// init plugins of server
// the service parameters is passed by startup paramters of prefix with -D
// -D servicename(library name)=service paramers
// or [D]service-name=paramters in file
// return 0 parse successfully
// return -1 parse error
int init_plugin(int argc,char ** argv)
{
	if(argc<=1)
	{
		logW(TAG," no plugin parameters ");
	}
	//load all service

	while(argc>1 && argv!=NULL && *++argv!=NULL)
	{
		char * pPar=*argv;	
		char * pVal=*++argv;
		int pos=findSeparator(pVal,"=");	
		if(pPar == NULL || *pPar != '-' || *++pPar=='\0' || pVal==NULL || *pVal=='\0')
		{
			logW(TAG," plugin initializer parse parameter error:par:%s  val:%s", pPar, pVal);
			continue;
		}
		// parameter is context parameter, isn't plugin paramters
		if(pos ==-1)
		{
			continue;
		}
		
		
		logI(TAG,"paramters count:%d get parameter:%s %s",argc,pPar,pVal);
		// this parameters is service parameters
		if(*pPar=='D')
		{
			char  serviceName[pos+1];
			memset(serviceName,0,pos+1);
			memcpy(serviceName,pVal,pos);
			serviceName[pos]='\0';
			char * parameter=&pVal[pos+1];
			logI(TAG," service:%s parameter:%s",serviceName,parameter);
			setServiceCreateParametersByN(serviceName,parameter);
		}
	}


	// create thread for that invoke all create function of service
	// FIXME create server sometimes crash , shoule be fork new process to do it
	pthread_t ptid;
	pthread_create(&ptid,NULL,createAllServices,NULL);
	return 0;
}


//start to listen
void startup()
{
	startListen(g_context,g_clients,g_services);
}


#ifdef DEBUG 

#include <assert.h>
void testReadConfFile(const char * path)
{
	__read_conf_file(path);
	assert(g_context->pServer->port==6000);
}

#endif


