

#include "tg_event.h"
#include <stdlib.h>
#include "tg_log.h"

#define TAG "EVENT"

static struct TGEvent * pHead=NULL;
static struct TGEvent * pTail=NULL;
static int queueCount=0;



// push new event to queue
//	0 client quit
//	1 client request
// Note: the event doesn't copy pData
//		only point to this. 
void pushEventToQueue(int type,void * pData)
{
	struct TGEvent * pEv=(struct TGEvent *) malloc(sizeof(struct TGEvent));
	memset(pEv,0,sizeof(struct TGEvent));
	//TODO generate toke
	pEv->token=-1;
	pEv->type=type;
	pEv->data=pData;

	if(pHead==NULL)
	{
		pHead=pEv;
		pTail=pEv;
	}
	else
	{
		pTail->pNext=pEv;
		pTail=pEv;
	}
	pTail->pNext=NULL;
	//record queue elements counts
	queueCount++;
}


// returned head of event queue
// event object must be free after use
struct TGEvent * head()
{
	struct TGEvent * pEv=pHead;
	if(pHead!=NULL)
	{
		pHead=pHead->pNext;	
		// check if head is null
		// it means current queue only exist one element
		// we shoule clean pTail
		if(pHead==NULL)
		{
			pTail=NULL;
		}
		queueCount--;
	}

	if(pEv!=NULL)
	{
		logD(TAG," event[%d  type:%d parameters:%s]",(long)pEv,pEv->type,(char *)pEv->data);
	}
	return pEv;
}


// return current queue size
int getQueueSize()
{
	return queueCount;
}


// cancel all cancel all events 
void cancelAllEvent()
{
	struct TGEvent * pEv=NULL;
	while((pEv=head())!=NULL)
	{
		freeTGEvent(pEv);
	}

}


// free struct Event memory
// Attribute pData of struct TGEvent as char * free 
void freeTGEvent(struct TGEvent * pEv)
{
	if(pEv!=NULL)
	{
		if(pEv->data!=NULL)
		{
			free((char *)pEv->data);
		}
		free(pEv);
	}
}


