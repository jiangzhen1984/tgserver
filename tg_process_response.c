

#include "tg_process_response.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tg_context.h"
#include "tg_service.h"
#include "tg_event.h"
#include "tg_log.h"

#define TAG "RESPONSE"

#define HANDLE_RESPONSE_SUCCESS 0

#define HEADER "tg://"


#define CHECK_EVENT(pEv) \
		if((pEv)==NULL || (pEv)->type>MSG_TYPE_MAX || (pEv)->type<MSG_TYPE_BASE) \
		{		\
			logE(TAG," event error:%d  event type max:%d   event type min:%d ",(pEv)==NULL?0:(pEv)->type,MSG_TYPE_MAX,MSG_TYPE_BASE);\
			return -1; \
		}		



#define SEND(pCli,ID)  if((ID)>=RESPONSE_MAX) { \
		logE(TAG," unknown RESPONSE ID:%d",(ID)); \
			return -1; \
		}; \	
	char * response=g_handle_response_msg[(ID)-RESPONSE_BASE];\
	int wt=write((pCli)->sockfd,response,strlen(response));\
	logI(TAG," send data length:%d",wt);



// define function pointer array
// Note: the postion of function pointer must equal (MSG_TYPE_.. - MSG_TYPE_BASE)
const static RESPONSE_FUNCTION g_handle_response_fp[]={	
		handle_response_client_quit,
		handle_response_client_request,
		handle_response_data_length_error,
		handle_response_server_shut_down,
		handle_response_client_init,
		handle_response_client_conn_closed_by_admin
		};


const static char * g_handle_response_msg[]={
		"tg://2000/",//RESPONSE_SEND_DATA
		"tg://2001/",//RESPONSE_DATA_LENGTH_ERROR
		"tg://2002/",//RESPONSE_REQUIRE_STR_ILLEGAL
		"tg://2003/",//RESPONSE_SERVER_SHUT_DOWN
		"tg://2004/",//RESPONSE_REQUIRE_SERVICE_NOT_FUOND
		"tg://2005/",//RESPONSE_MAX_CONNECTIONS
		"tg://2006/",//RESPONSE_RETURNED_DATA_FORMAT_ILLEGAL	
		"tg://2007/"//#define RESPONSE_CONN_CLOSED_BY_ADMIN 2007
		};


// check client send request string 
// if it is correct and return TGService object, return 0 
// if not found service that client requested, then return -1
// if requested string is illegal then return 1
// Note: if check successfully you must free TGService object after use
static int __check_request(struct TGService ** ppReq,char * pReq)
{
	int ret=1;
	//FIXME should use regex for match
	int len=strlen(HEADER);
	if(strncmp(pReq,HEADER,len)==0)
	{
		int pos=findSeparator(&pReq[len],"/");
		if(pos!=-1)
		{
			char name[pos-len];
			strncpy(name,&pReq[len],pos);
			struct TGService * pReqService=findServiceN(name);
			if(pReqService!=NULL)
			{
				*ppReq=pReqService;
				ret=0;
			}
			else
			{
				ret=-1;
			}	
		}
	}
	

	return ret;
}




// handle response event function 
int handle_response(TGClient *pTGCli,struct TGEvent *pEv)
{
	// check event type is legal or not 
	// if is illegal, then return -1
	CHECK_EVENT(pEv)
	logI(TAG," event type:%d  parameters:%s",pEv->type,pEv->data);
	return g_handle_response_fp[pEv->type-MSG_TYPE_BASE](pTGCli,pEv);
}

// handle response event for client quit
int handle_response_client_quit(TGClient *pTGcli,struct TGEvent * pEv)
{

	releaseAllServices();

	destroyClient(pTGcli);
	release();
	// exit child process	
	exit(0);
	return  HANDLE_RESPONSE_SUCCESS;
}

// handle response event for client request
int handle_response_client_request(TGClient *pTGcli,struct TGEvent * pEv)
{
	// check event type is legal or not 
	// if is illegal, then return -1
	CHECK_EVENT(pEv)

	struct TGService * pSer=NULL;
	int ret;
	if((ret=__check_request(&pSer,pEv->data))==0)
	{
		if(pSer!=NULL)
		{
			// call service function for handle request 
			logI(TAG," request service:%s",pSer->serviceName);
			char * pData=(char *)pSer->do_request(pEv->data);
			if(pData!=NULL)
			{
				
				int dataSize=(unsigned char )pData[3]<<24 | (unsigned char )pData[2]<<16 | (unsigned char)pData[1]<<8 | (unsigned char)pData[0];
				if(dataSize<=0)
				{
					logE(TAG,"The returned data error: first four bytes is not integer ");
					//TODO send error message
					SEND(pTGcli,RESPONSE_RETURNED_DATA_FORMAT_ILLEGAL)

				}
				else
				{
					logD(TAG,"get data size:%d",dataSize);
					char * header=g_handle_response_msg[0];
					int headerSize=4+strlen(header);
					int allSize=headerSize +dataSize;
					//char * sendData=(char *)malloc(allSize);

					char  * sendData=(char *)malloc(headerSize);
					memset(sendData,0,headerSize);
					memcpy(sendData,&allSize,4);
					memcpy(&sendData[4],header,strlen(header));

					int rt2=write(pTGcli->sockfd,sendData,headerSize);
					logI(TAG," send header data to client status:%d header size:%d ",rt2,headerSize);
					logM(sendData,headerSize);
					free(sendData);

					rt2=write(pTGcli->sockfd,&pData[4],dataSize);
					logI(TAG," send  data to client status:%d data size:%d ",rt2,dataSize);
					logM(&pData[4],dataSize);

				
				}
				free(pData);
			}
		}
		else
		{
			logE(TAG,"unknow error: service is NULL");
			ret=-1;
		}
	}
	else if(ret==1)
	{

		logE(TAG," required data illegal:%s ",pEv->data);
		SEND(pTGcli,RESPONSE_REQUIRE_STR_ILLEGAL)
	}
	else
	{
		logE(TAG," required service not found:%s",g_handle_response_msg[RESPONSE_REQUIRE_SERVICE_NOT_FOUND-RESPONSE_BASE]);
		SEND(pTGcli,RESPONSE_REQUIRE_SERVICE_NOT_FOUND)

	}
	if(pSer!=NULL)
	{
		free(pSer);	
        pSer=NULL;
	}

	return  ret;
}

// handle response event for that client sended data length error
int handle_response_data_length_error(TGClient *pTGcli,struct TGEvent * pEv)
{
	logD(TAG,"send data:%s",g_handle_response_msg[RESPONSE_DATA_LENGTH_ERROR-RESPONSE_BASE]);
	SEND(pTGcli,RESPONSE_DATA_LENGTH_ERROR)
	return  HANDLE_RESPONSE_SUCCESS;
}

// handle response event for server shut down
int handle_response_server_shut_down(TGClient *pTGcli,struct TGEvent * pEv)
{
	SEND(pTGcli,RESPONSE_SERVER_SHUT_DOWN)

	destroyClient(pTGcli);
	// release memory
	release();
	sleep(2);
	// exit child process	
	exit(0);
	return  HANDLE_RESPONSE_SUCCESS;
}

// handle response event fot that client connection is established
int handle_response_client_init(TGClient *pTGcli,struct TGEvent * pEv)
{
	initAllServices();
	return HANDLE_RESPONSE_SUCCESS;
}

// handle response event for that client connection was closed by administrator
int handle_response_client_conn_closed_by_admin(TGClient *pTGcli,struct TGEvent * pEv)
{
	SEND(pTGcli,RESPONSE_CONN_CLOSED_BY_ADMIN)

	destroyClient(pTGcli);
	// release memory
	release();
	sleep(2);
	// exit child process	
	exit(0);

	return HANDLE_RESPONSE_SUCCESS;
}


