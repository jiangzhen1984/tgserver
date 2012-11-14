

#include "tg_context.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tg_log.h"
#include "tg_util.h"

#ifdef LINUX
#include <sys/socket.h>
#include <errno.h>
#elif WIN32
#endif


#define CAL_CLIENT_ARRAY_SIZE clients_ar_size
#define TAG "CONTEXT"

TGClient ** g_clients=NULL;
static int clients_ar_size=0;


TGContext * g_context=NULL;


static void releaseClient(TGClient * pClient)
{
	if(pClient!=NULL)
	{
		if(pClient->sockfd>=0)
		{
			logI(TAG," shut down client connection:%s",pClient->clientIP);
#ifdef LINUX
			shutdown(pClient->sockfd,SHUT_RDWR);
#elif WIN32
			close(pClient->sockfd);
#endif
		}
		
		if(pClient->readPipe>=0)
		{
			close(pClient->readPipe);
		}
		if(pClient->writePipe>=0)
		{
			close(pClient->writePipe);
		}
		if(pClient->clientIP!=NULL)
		{
			free(pClient->clientIP);
		}
		memset(pClient,0,sizeof(TGClient));
		free(pClient);
	}
}


static void releaseClientArray()
{
	//release old array
	TGClient * pT;
	int i=0;
	for(i=0;i<clients_ar_size;i++)
	{
		pT=g_clients[i];
		if(pT!=NULL)
		{
			releaseClient(pT);
		}	
	}
	
	memset(g_clients,0,clients_ar_size * sizeof(TGClient *));
	free(g_clients);
	g_clients=NULL;

}


static void appendClient()
{
	int newClientCounts=clients_ar_size+DEFAULT_MAX_CLIENTS;
	int size=sizeof(TGClient*) * newClientCounts;
	TGClient ** ppNew=(TGClient **) malloc(size);
	memset(ppNew,0,size);
	logI(TAG,"append client poll:%d old size:%d address from:%d to:%d",newClientCounts,clients_ar_size,(long )&ppNew[0],(long)&ppNew[newClientCounts-1]);
	if(g_clients!=NULL)
	{

		memcpy(ppNew,g_clients,clients_ar_size*sizeof(TGClient *));
		free(g_clients);
	}
	g_clients=ppNew;
	clients_ar_size=newClientCounts;
}


void init()
{

/**	int size=sizeof(TGClient*) * DEFAULT_MAX_CLIENTS;
	clients_ar_size=DEFAULT_MAX_CLIENTS;
	g_clients=(TGClient **) malloc(size);
	memset(g_clients,0,size);
	
**/
	//init client array
	appendClient();
	logI(TAG," server clients initialiezd");

	// init context
	int size=sizeof(TGContext);
	g_context=(TGContext *)malloc(size);
	memset(g_context,0,size);
	logI(TAG," server context initialized");
	
	size=sizeof(TGServer);
	g_context->pServer=(TGServer *)malloc(size);
	memset(g_context->pServer,0,size);
	// init default value
	g_context->pServer->port=DEFAULT_LISTEN_PORT;
	g_context->pServer->maxClients=DEFAULT_MAX_CLIENTS;
	logI(TAG," server info initialized");
}
	


void release()
{
	if(g_clients!=NULL)
	{
		logD(TAG," release g_clients");
		releaseClientArray();
		g_clients=NULL;
	}

	if(g_context!=NULL)
	{
		logD(TAG," release g_context");
		if(g_context->pServer!=NULL)
		{
			if(g_context->pServer->listen_ip!=NULL)
			{
				free(g_context->pServer->listen_ip);
			}

			if(g_context->pServer->name!=NULL)
			{
				free(g_context->pServer->name);
			}
			memset(g_context->pServer,0,sizeof(TGServer));

			free(g_context->pServer);
		}
		if(g_context->listenfd>=0)
		{
#ifdef LINUX
			shutdown(g_context->listenfd,SHUT_RDWR);
#endif
			close(g_context->listenfd);
		}

		if(g_context->localAdminfd>=0)
		{
			close(g_context->localAdminfd);
		}
		memset(g_context,0,sizeof(TGContext));
		free(g_context);
		g_context=NULL;
	}
	clients_ar_size=0;
	logD(TAG," finish release ");
}



void initClient(TGClient ** ppClient)
{
	TGClient * pCli=(TGClient *) malloc(sizeof(TGClient));
	pCli->pid=0;
	pCli->sockfd=-1;
	pCli->readPipe=-1;
	pCli->writePipe=-1;
	pCli->clientIP=NULL;
	pCli->port=0;
	*ppClient=pCli;
	
}


// find client in g_clients array
// according to process id. if find it, set pointer to ppClient.
// return 0 SUCCESS and find it
// reutrn -1 not found client 
int findClient(tg_pid pid, TGClient ** ppClient)
{

	int i=0;
	int count=CAL_CLIENT_ARRAY_SIZE;
	int flag=FIND_CLIENT_NOT_FOUND;
	for(i=0;i<count;i++)
	{
		TGClient * pC=g_clients[i];
		if(pC!=NULL && pC->pid==pid)
		{
			*ppClient=pC;	
			flag=FIND_CLIENT_SUCCESS;
			break;
		}

	}
	return flag;

}


// add client to global clients array(g_clients)
// copy client
void addClient(TGClient * pClient)
{
	if(pClient==NULL)
	{
		return;
	}

	int count=CAL_CLIENT_ARRAY_SIZE; 

	TGClient * pT=NULL;
	initClient(&pT);
	memcpy(pT,pClient,sizeof(TGClient));
	

	int flag=0;
	int i=0;
	for(i=0;i<count;i++)
	{
		TGClient * pC=g_clients[i];
		if(pC==NULL)
		{
			g_clients[i]=pT;
			flag=1;
			break;
		}
	}

	if(!flag)
	{
		logN("CONTEXT",LOG_LEVEL_DEBUG,"start create client");
		int currentPos=clients_ar_size;
		appendClient();
		g_clients[currentPos]=pT;
	}
	// record current client connection counts	
	g_context->curClientCounts++;

}


// close client connection
//  and remove client from  global clients array
void closeClient(tg_pid pid)
{
	int i=0;
	int count=CAL_CLIENT_ARRAY_SIZE;
	for(i=0;i<count;i++)
	{
		TGClient * pC=g_clients[i];
		if(pC!=NULL && pC->pid==pid)
		{
			g_clients[i]=NULL;
			releaseClient(pC);
			// record current client connection counts	
			g_context->curClientCounts--;
			return;
		}
	}

}


void closeAllClient()
{
	int i=0;
	int count=CAL_CLIENT_ARRAY_SIZE;
	for(i=0;i<count;i++)
	{
		TGClient * pC=g_clients[i];
		if(pC!=NULL)
		{
			logN("CONTEXT",LOG_LEVEL_DEBUG," opid:%d close client: addres:%d pid:%d, port:%d",getTGServerPid(),pC,pC->pid,pC->port);
			releaseClient(pC);
			g_clients[i]=NULL;
		}

	}

}


int getCurrentClientCounts()
{
	if(g_clients==NULL)
	{
		logE(TAG,"clients array isn't initialized!");
		return 0;
	}	
	int count=0;
	TGClient * pT=NULL;
	TGClient ** pHead=g_clients;
	while((pT=*pHead++)!=NULL)
	{
		count++;
	}
	return count;
}
	

// release client memory
// close client all sock descriptior
void destroyClient(TGClient * pCli)
{
	if(pCli!=NULL)
	{
		releaseClient(pCli);
	}
}

// send signal to client
// return 0 send successfully
// return -1 send error client pipe closed
int sendSignalToClient(TGClient * pCli,char * msg)
{
	if(pCli==NULL || msg==NULL || pCli->writePipe<0)
	{
		return -1;
	}
	int n=write(pCli->writePipe,msg,strlen(msg));
	if(n==-1)
	{
#ifdef LINUX
		logE(TAG," send signal to sub process failed:%s",strerror(errno));
#elif WIN32
#endif

		return -1;
	}
	return 0;
	
}

// send signal to all activite clients
// return 0 send successfully
// return >0 that failed client counts
int sendSignalToAllClients(char * msg)
{
	int errCount=0;
	int i=0;
	int count=CAL_CLIENT_ARRAY_SIZE;
	for(i=0;i<count;i++)
	{
		TGClient * pC=g_clients[i];
		if(pC!=NULL)
		{
			int r=sendSignalToClient(pC,msg);
			if(r!=0)
			{
				errCount++;	
			}
		}

	}

	return errCount;
}


// get current connected client
// and return every client one line
// Note: after use data, must free ppClientInfo data
//  pid   ip  port
void getCurrentClientsInfo(char ** ppCliInfo)
{
	int i=0;
	int count=CAL_CLIENT_ARRAY_SIZE;
	char * pData=(char *)malloc(50*count);
	memset(pData,0,50*count);
	int pos=0;
	for(i=0;i<count;i++)
	{
		TGClient * pC=g_clients[i];
		if(pC!=NULL)
		{
			char buf[50];
			memset(buf,0,50);			
			int p=sprintf(buf,"%d\t\t%s\t\t%d\r\n",pC->pid,pC->clientIP,pC->port);
			memcpy(&pData[pos],buf,p);
			pos+=p;
		}
	}
	*ppCliInfo=pData;
}

