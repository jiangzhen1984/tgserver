

#include "../tg_event.h"
#include <stdlib.h>
#include <assert.h>


void finish()
{
	while(head()!=NULL);
}

void testPushEventToQueue()
{
	pushEventToQueue(1,NULL);
	pushEventToQueue(2,NULL);
	pushEventToQueue(3,NULL);
	pushEventToQueue(4,NULL);

	int size=getQueueSize();
	assert(size==4);
	
	struct TGEvent * pEv=head();
	assert(pEv!=0);
	assert(pEv->type==1);
	size=getQueueSize();
	assert(size==3);

	finish();
	printf(" testPushEventToQueue()  ok\n");
}


void testHead()
{
	pushEventToQueue(1,NULL);
	struct TGEvent * pEv=head();
	assert(pEv!=0);
	assert(pEv->type==1);

	struct TGEvent * pEv1=head();
	assert(pEv1==0);

	pushEventToQueue(1,NULL);
	struct TGEvent * pEv2=head();
	assert(pEv2!=0);
	assert(pEv2->type==1);
	finish();
	printf(" testHead()  ok\n");
}


void testGetQueueSize()
{
	pushEventToQueue(1,NULL);
	assert(getQueueSize()==1);

	pushEventToQueue(2,NULL);
	assert(getQueueSize()==2);

	pushEventToQueue(3,NULL);
	assert(getQueueSize()==3);

	pushEventToQueue(4,NULL);
	assert(getQueueSize()==4);
	
	head();
	assert(getQueueSize()==3);
	head();
	assert(getQueueSize()==2);
	head();
	assert(getQueueSize()==1);
	head();
	assert(getQueueSize()==0);

	finish();
	printf(" testGetQueueSize()  ok\n");
	
}

int main(int argc,char ** argv)
{
	testPushEventToQueue();

	testHead();

	testGetQueueSize();
}
