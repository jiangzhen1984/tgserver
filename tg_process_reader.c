
#include "tg_process_reader.h"
#include "tg_process_response.h"
#include "tg_context.h"
#include "tg_log.h"


#define TAG "PROCESS-READER"



// read data from pClient
// return 0 handle successfully
// return -1 read data  failed 
// return 1 read data successfully but content that client sent is error 
// pData is used to malloc function to allocate
// and isn't necessary free because deamon process thread will be free
int read_data_from_cli(TGClient *pClient, int *pType, char** ppData)
{
	if(pClient==NULL || pClient->sockfd<0)
	{
		return -1;
	}
	int sockfd=pClient->sockfd;
	int clientReqType;
	char * pData=NULL;
	char buf[255];
	//FIXME shoule be check sockfd 
	int n=read(sockfd,buf,sizeof(buf));
	if(n==0 || n==-1 || n<4)
	{
		goto L_EXIT;
	}
	else
	{
		char sizeBuf[4];
		memset(sizeBuf,0,sizeof(sizeBuf));
		memcpy(sizeBuf,buf,sizeof(sizeBuf));
		int size=(unsigned char)sizeBuf[3] << 24 | (unsigned char )sizeBuf[2] << 16 | (unsigned char )sizeBuf[1] << 8 | (unsigned char)sizeBuf[0];

		int readLen=n-4;
		int lastLen=size-readLen;

		logI(TAG,"get total data length:%d  read length:%d",size,(n-4));
		if(size<=0)
		{
			*pType=MSG_TYPE_DATA_LENGTH_ERROR;
			logI(TAG,"read data length error:%s",buf);
			return 1;
		}
		else if(lastLen>0)
		{
			// Note: it's not necessary to free
			// because  deamon response thread will be free it
			char * pDataBuf=(char *)malloc(size+1);
			memset(pDataBuf,0,size+1);
			// copy from fifte byte ,because first 4 bytes is data lenth not data
			memcpy(pDataBuf,&buf[4],readLen);
			int n1=0;
			while(lastLen>0)
			{
				n1=read(sockfd,&pDataBuf[readLen],lastLen);
				if(n1==0 || n1==-1)
				{
					free(pDataBuf);
					goto L_EXIT;
				}
				lastLen-=n1;
				readLen+=n1;
			}
			pData=pDataBuf;
			*pType=MSG_TYPE_CLIENT_REQUEST;
		}
		else
		{
			// Note: it's not necessary to free
			// because  deamon response thread will be free it
			char * pDataBuf=(char *)malloc(size+1);
			memset(pDataBuf,0,size);
			memcpy(pDataBuf,(char *)&buf[4],size);
			pData=pDataBuf;
			*pType=MSG_TYPE_CLIENT_REQUEST;
		}

		logI(TAG,"request data:%s",pData);
		*ppData=pData;
	}
	return 0;

L_EXIT:
	logI(TAG," %d server exit cause by :%d",getTGServerPid(),n);
	// send thread notification signal
	*pType=MSG_TYPE_CLIENT_QUIT;
	ppData=NULL;
	return 0;
}


// read data from main process pipe
// return 0 handle successfully
// return -1 read data  failed 
// return 1 read data successfully but content that client sent is error 
// pData is used to malloc function to allocate
int  read_data_from_main_process(TGClient *pClient,int *pType, char** ppData)
{
	if(pClient==NULL || pClient->readPipe<0)
	{
		return -1;
	}
	char buf[255];
	memset(buf,0,sizeof(buf));
	//FIXME shoule be check pipe
	int n=read(pClient->readPipe,buf,sizeof(buf));
	if(n==0)
	{
		logE(TAG,"main pipe[%d] closed",pClient->readPipe);
		return -1;
	}
	else if(n==-1)
	{
		char * msg="";
		GET_ERR_MSG(msg)
		//FIXME error
		//logE(TAG," read from pipe error:%s",msg);
		return -1;
	}

	// main process send msg:  tg://id/
	char no[4];
	memset(no,0,4);
	strncpy(no,&buf[5],4);
	int msgID=atoi(no);
	*pType=msgID;
	if(msgID<MSG_TYPE_BASE || msgID>MSG_TYPE_MAX)
	{
		logE(TAG," unkonwn msg type:%d",msgID);
		return 1;
	}
	logI(TAG," read event from main process type:%d",msgID);
	return 0;
	
}


