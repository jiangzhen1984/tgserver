
#include "tg_process.h"

#ifdef LINUX
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/errno.h>
// used to communicates with local admin shell script
// when read data from /tmp/tgp and write data to /tmp/tgo
#define PIPE "/tmp/tgp"
// used to communicates with local admin shell script
// when read data from /tmp/tgp and write data to /tmp/tgo
#define OUT_PIPE "/tmp/tgo"
#elif WIN32
#define PIPE 
#endif


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "tg_context.h"
#include "tg_log.h"
#include "tg_event.h"
#include "tg_util.h"
#include "tg_process_response.h"
#include "tg_process_reader.h"
#include "tg_process_admin.h"


#define TAG "PROCESS"
#define BUF_SIZE 255



#ifdef LINUX

// define lock for event queue
static pthread_mutex_t event_mutex=PTHREAD_MUTEX_INITIALIZER;
// define condition for event queue
static pthread_cond_t  event_cond=PTHREAD_COND_INITIALIZER;


static void __send_thread_signal(int type,char * pData)
{
	logI(TAG," push event:%d data:%s",type,pData);
	logD(TAG," start to get lock");
	pthread_mutex_lock(&event_mutex);
	pushEventToQueue(type,pData);
	pthread_cond_signal(&event_cond);
	pthread_mutex_unlock(&event_mutex);
	logD(TAG," finish to  unlock");
}


static void __deamon_read(TGClient  * pClient)
{
	// send event for client init
	__send_thread_signal(MSG_TYPE_CLIENT_INIT,NULL);

	int sockfd=pClient->sockfd;
	int mPipe=pClient->readPipe;
	fd_set allSet,rSet;

	FD_ZERO(&allSet);
	FD_SET(sockfd,&allSet);
	FD_SET(mPipe,&allSet);
	int maxfd=sockfd>mPipe?sockfd:mPipe;

	for(;;)
	{
		rSet=allSet;
		int rt=select(maxfd+1,&rSet,NULL,NULL,NULL);
		if(rt>0)
		{
			// check client sock is ready for read or not 
			if(FD_ISSET(sockfd,&rSet)>0)
			{
				int msgType=0;
				char * pData=NULL;
				//read data  from client
				if(read_data_from_cli(pClient,&msgType,&pData)!=-1)
				{
					__send_thread_signal(msgType,pData);
				}
			}

			if(FD_ISSET(mPipe,&rSet)>0)
			{
				int msgType=0;
				char * pData=NULL;
				if(read_data_from_main_process(pClient,&msgType,&pData)!=-1)
				{
					__send_thread_signal(msgType,pData);
				}
			}

		}
		else if(errno==EINTR)
		{
			logI(TAG," client exit");
			break;
		}
	}


}


static void __deamon_response(TGClient * pClient)
{
	for(;;)
	{
		//struct TGEvent * pEv=NULL;
		pthread_mutex_lock(&event_mutex);
		//get first request
		struct TGEvent * pEv=head();
		if(pEv==NULL)
		{
			pthread_cond_wait(&event_cond,&event_mutex);
			pthread_mutex_unlock(&event_mutex);
			continue;
		}
		pthread_mutex_unlock(&event_mutex);
		// handle event
		int ret=handle_response(pClient,pEv);
		logI(TAG," HANDLE RESPONSE RESULT:%d, EVENT[type:%d parameters:%d]",ret,pEv->type,pEv->data);
		// free TGEvent memory	
		freeTGEvent(pEv);
	}
}

// create two threads for client connection
// one is read data from sockfd
// another one is write data to sockfd
// both two threads communicates each other
// with event queue and event_cond and event_mutex
static void __do_request(TGClient * pClient)
{
	initAllServices();
	//delay for init service
	sleep(1);
	pthread_t child_tid;
	pthread_t child_tid_response;
	// create thread for request handle
	pthread_create(&child_tid,NULL,__deamon_read,(void *)pClient);
	// create thread for response request
	pthread_create(&child_tid_response,NULL,__deamon_response,(void *)pClient);
#ifdef LINUX
	while(1)
	{
		sleep(0xffffff);
	}
#elif WIN32
#endif
}





static void setPipe(TGClient * pCli,int readfd,int writefd)
{
	if(readfd!=-1)
	{
		pCli->readPipe=readfd;
		logI(TAG," set read pipe:%d",readfd);
	}
	else
	{
		logE(TAG," Can not set read pipe for main process:%d",readfd);
	}

	if(writefd!=-1)
	{
		pCli->writePipe=writefd;
		logI(TAG," set write pipe:%d",writefd);
	}
	else
	{
		logE(TAG," Can not set write pipe for main process:%d",writefd);
	}
	
}


// save client information to TGClient
static void setClient(TGClient * pCli,struct sockaddr_in * pClientAddr)
{
	pCli->port=pClientAddr->sin_port;
	char str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET,&pClientAddr->sin_addr,str,sizeof(str));
	int ipSize=strlen(str)+1;
	char * buf=(char *)malloc(ipSize);
	memset(buf,0,ipSize);
	strncpy(buf,str,ipSize-1);
	buf[ipSize]='\0';
	pCli->clientIP=buf;
	logI(TAG," CLIENT[%s:%d] established connection",pCli->clientIP,pCli->port);
	

}

static int do_accept(TGContext * pContext,TGClient ** ppArClient)
{
	logI(TAG," current client counts:%d  max client counts:%d",pContext->curClientCounts,pContext->pServer->maxClients);
	struct sockaddr_in client_addr;
	int size=sizeof(struct sockaddr_in);
	int sockfd=accept(pContext->listenfd,&client_addr,&size);
	if(sockfd==-1)
	{
		logE(TAG," accept error:%s",strerror(errno));
		return -1;	
	}

	//TODO open pipe for that child and root process communicate each other
	TGClient * pClient;
	initClient(&pClient);
	// copy client info to TGClient 
	setClient(pClient,&client_addr);

	//define description for read pipe
	int fd[2];
	//define description for write pipe
	int fd1[2];

	pipe(fd);
	pipe(fd1);

	
	tg_pid cpid;
	// child process
	if((cpid=fork())==0)
	{
		pClient->sockfd=sockfd;
		close(fd1[0]);
		close(fd[1]);
		setPipe(pClient,fd[0],fd1[1]);
		//clean sock in child process
		closeAllClient();
		close(pContext->listenfd);
		close(pContext->localAdminfd);
		//FIXME check max connection  should check in main process 
		if(pContext->curClientCounts>= pContext->pServer->maxClients)
		{
			// child process exit
			exit(0);
		}
		__do_request(pClient);
		return 0;
	}
	else if (cpid>0)
	{
		close(sockfd);
		close(fd1[1]);
		close(fd[0]);
		pClient->pid=cpid;
		setPipe(pClient,fd1[0],fd[1]);
		addClient(pClient);
		return 0;
	}
	else
	{
		logE(TAG," create child process error%d",cpid);
		return -1;
	}
	
}


// wrap native socket function 
// just check error
static int socket_w(TGContext * pContext)
{
	int listenfd;
	listenfd=socket(AF_INET,SOCK_STREAM,0);
	if(listenfd==-1)
	{
		logE(TAG,"can not create sock");
		exit(1);
	}
	pContext->listenfd=listenfd;
	return listenfd;
}


// wrap native bind function 
// just check error
static int bind_w(TGContext * pContext)
{
	char str[INET_ADDRSTRLEN];
	struct sockaddr_in  pServer_addr;
	pServer_addr.sin_family=AF_INET;
	pServer_addr.sin_port=htons(pContext->pServer->port);
	if(pContext->pServer->listen_ip!=NULL && *(pContext->pServer->listen_ip) !='\0')
	{
		pServer_addr.sin_addr.s_addr=inet_addr(pContext->pServer->listen_ip);
	}
	else
	{
		pServer_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	}

	inet_ntop(AF_INET,&pServer_addr.sin_addr,str,sizeof(str));
	

	int rt=bind(pContext->listenfd,(const struct sockaddr_in *)&pServer_addr,sizeof(struct sockaddr_in));
	if(rt==-1)
	{
		logE(TAG," bind sock error:%d address:%s  errstr:%s",errno,str, strerror(errno));	
		exit(1);
	}

	logI(TAG," bind address:%s  listen:%d",str,pContext->pServer->port);
	return rt;
}


// wrap native listen function 
// just check error
static int listen_w(TGContext * pContext)
{
	//FIXME the second parameter what to do?
	int rt=listen(pContext->listenfd,0);
	if(rt==-1)
	{
		logE(TAG," bind sock error:%s",strerror(errno));	
		exit(1);
	}
	
	return rt;
}


static void __handle_child_quit_signal()
{
	pid_t pid;
	int status;
	while( (pid=waitpid(-1,&status,WNOHANG))>0 )
	{
		closeClient(pid);
#ifdef DEBUG
		logI(TAG," %d client send exit signal", pid);
#endif
	}
#ifdef DEBUG
		logI(TAG," signal handled");
#endif
	return;

}

static void __handle_host_shutdown_signal()
{
	logI(TAG," server shut down");
	//FIXME tell all client processes that host will be shutdown
	int n=sendSignalToAllClients("tg://1003/");
	if(n!=0)
	{
		logE(TAG," send shut down signal error count:%d",n);
	}
	int timer=10;
	while(timer-->0)
	{
		sleep(1);
		logW(TAG," server will shut down after %d seconds",timer);
	}

	//close all client connection in main process
	closeAllClient();
	// destroy all service	
	destroyAllServices();
	//release all global variable	
	release();
	exit(0);
}


// return opened pipe descriptor
static int open_admin_pipe()
{
	int fd=-1;
	if((fd=mkfifo(PIPE,0666))<0 && errno!=EEXIST)
	{
		logE(TAG," can not create pipe for administration error:%s",strerror(errno));
		return fd;
	}

	//Note: O_RDWR just for block fd
	fd=open(PIPE,O_RDWR|O_NONBLOCK,0);
	if(fd<0)
	{
		logE(TAG," can not open pipe for administration error:%s",strerror(errno));
		return fd;
	}
	return fd;
}


static int do_admin(TGContext * pContext)
{
	char buf[20];
	memset(buf,0,sizeof(buf));
	int n=read(pContext->localAdminfd,buf,sizeof(buf));
	logD(TAG,"read from pipe:%s   size:%d",buf,n);
	if(n>0)
	{
		char * cmd="tg://1003/";
		logD(TAG,"strcmp %s %d",cmd,strcmp(buf,cmd));
		if(strcmp(buf,cmd)==0)
		{
			unlink(PIPE);
			__handle_host_shutdown_signal();
		}
		else
		{
			int fd=-1;
			if((fd=mkfifo(OUT_PIPE,0666))<0 && errno!=EEXIST)
			{
				logE(TAG," can not create pipe for administration error:%s",strerror(errno));
			}

			//Note: O_RDWR just for block fd
			fd=open(OUT_PIPE,O_RDWR|O_NONBLOCK,0);
			if(fd<0)
			{
				logE(TAG," can not open pipe for administration error:%s",strerror(errno));
			}
			write_data(fd,buf);
		}
	}
	else
	{
		logE(TAG,"read error from pipe:%s error:%s",PIPE,strerror(errno));
	}
	return n;
}


#endif


// start to listen socket
// accept client connection and push connection to ppArClient
void startListen(TGContext * pContext,TGClient ** ppArClient,struct TGService ** ppArService)
{
#ifdef LINUX
	fd_set rset,allset;
	// create socket
	socket_w(pContext);

	//bind socket
	bind_w(pContext);
	
	//listen sock
	listen_w(pContext);


	// handle child process exit
	signal(SIGCHLD,__handle_child_quit_signal);
	// handle host shutdown signal
	signal(SIGTERM,__handle_host_shutdown_signal);

	FD_ZERO(&allset);
	FD_SET(pContext->listenfd,&allset);

	int fd=open_admin_pipe();
	if(fd!=-1)
	{
		pContext->localAdminfd=fd;
		FD_SET(pContext->localAdminfd,&allset);
	}

	// TODO create new pthread for listen sub process and write message to client

	int maxfd=pContext->localAdminfd>pContext->listenfd?pContext->localAdminfd:pContext->listenfd;
	for(;;)
	{
		rset=allset;
		// just listen listenfd
		int rt=select(maxfd+1,&rset,NULL,NULL,NULL);
		if(rt==-1 )//&& errno!=EINTR)
		{
			logE(TAG," select error errno:%d msg:%s",errno,strerror(errno));
			continue;
		}
		if(FD_ISSET(pContext->listenfd,&rset))
		{
			rt=do_accept(pContext,ppArClient);
		}
		if(FD_ISSET(pContext->localAdminfd,&rset))
		{
			rt=do_admin(pContext);
		}
	}
	

#elif WIN32

#endif

}

#ifdef DEBUG

void testListen()
{
}


void testThread()
{
}

#endif



