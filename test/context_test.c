
#include <assert.h>
#include "../tg_context.h" 


void prepare()
{
	init();
}

void finish()
{
	release();
}

void testInit()
{
	//test init
	init();
	assert(g_context!=NULL);
	assert(g_clients!=NULL);
	assert(g_services!=NULL);
	printf("testInit() ok\n");
	finish();

}

void testRelease()
{
	init();
	release();
	assert(g_context==NULL);
	assert(g_clients==NULL);
	assert(g_services==NULL);
	printf("testRelease() ok\n");
	finish();
}




void testAddClient()
{
	printf("start test\n");
	prepare();
	int i=100;
	for(i=100;i<210;i++)
	{
		TGClient * pT=NULL;
		initClient(&pT);
		pT->pid=i;
		addClient(pT);
	}

	TGClient * pTGC=NULL;
	int ret=findClient(105,&pTGC);
	assert(ret==0);
	assert(pTGC!=NULL);
	printf("find client:%d\n",pTGC->pid);
	finish();
	printf("testAddClient() ok\n");

}


void testGetCurrentClientCounts()
{
	prepare();
	int count=getCurrentClientCounts();	
	assert(count==0);
	printf("testGetCurrentClientCounts() count:%d\n",count);
	int i=100;
	for(i=100;i<210;i++)
	{
		TGClient * pT=NULL;
		initClient(&pT);
		pT->pid=i;
		addClient(pT);
	}
	count=getCurrentClientCounts();
	assert(count==110);
	printf("testGetCurrentClientCounts() count:%d\n",count);
	
	finish();
	printf("testGetCurrentClientCounts() ok\n");
}

void testFindClient()
{
	prepare();
	int i=100;
	for(i=100;i<101;i++)
	{
		TGClient * pT=(TGClient *) malloc(sizeof(TGClient));
		pT->pid=i;
		addClient(pT);
	}

	TGClient * pTGC=NULL;
	int ret=findClient(100,&pTGC);
	assert(ret==0);
	assert(pTGC!=NULL);
	printf("find client:%d\n",pTGC->pid);

	finish();
	printf("testFindClient() ok\n");
}




void testCloseClient()
{
	prepare();
	TGClient * ptg=(TGClient *) malloc(sizeof(TGClient));
	ptg->pid=100;
	addClient(ptg);

	closeClient(100);
	TGClient * pTGC=NULL;
	int ret=findClient(100,&pTGC);
	assert(ret==-1);
	assert(pTGC==NULL);
	finish();

	printf("testCloseClient() ok\n");
}

void testCloseAllClient()
{
	prepare();

	TGClient * ptg=(TGClient *) malloc(sizeof(TGClient));
	ptg->pid=100;
	addClient(ptg);
	assert(g_clients[0]!=NULL);
	assert(g_clients[0]->pid==100);

	TGClient * ptg1=(TGClient *) malloc(sizeof(TGClient));
	ptg1->pid=101;
	addClient(ptg1);
	assert(g_clients[1]!=NULL);
	assert(g_clients[1]->pid==101);

	closeAllClient();
	assert(g_clients[1]==NULL);
	assert(g_clients[10]==NULL);

	finish();
	printf("testCloseAllClient() ok\n");

}


int main(int argc,char ** argv)
{
	setenv("LOG_LEVEL","3");
	
//	testInit();
//	testRelease();
	
//	testAddClient();

	testGetCurrentClientCounts();
	testFindClient();


	
	testCloseClient();
	testCloseAllClient();

}
