

#include "../tg_server.h"
#include "../tg_context.h"
#include <assert.h>



void testInit_Context()
{
	release();
	char * par[11];
	par[0]="./server_test";
	par[1]="-p";
	par[2]="6000";
	par[3]="-l";
	par[4]="192.168.1.1";
	par[5]="-m";
	par[6]="200";
	par[7]="-D";
	par[8]="log=wefwe";
	par[9]='\0';
	int ret=init_context(11,&par);
	assert(ret==0);
	assert(g_context->pServer!=0);
	assert(g_context->pServer->port==6000);
	assert(strcmp(g_context->pServer->listen_ip,"192.168.1.1")==0);

	char * par2[8];
	par2[0]="./server_test";
	par2[1]="-p";
	par2[2]="6000";
	par2[3]="-"; 
	par2[4]="192.168.1.1";
	par2[5]="-m";
	par2[6]="200";
	par2[7]='\0';
	int ret2=init_context(8,&par2);
	assert(ret2==-1);
	assert(g_context==0);


	char * par5[7];
	par5[0]="./server_test";
	par5[1]="-p";
	par5[2]="6000";
	par5[3]="-";
	par5[4]="192.168.1.1";
	par5[5]="-m";
	par5[6]="200";
	int ret5=init_context(8,&par5);
	assert(ret5==-1);
	assert(g_context==0);


/**	
	char * par1[10];
	par1[0]="./server_test";
	int ret=init_context(10,&par);
	assert(ret==0);
	assert(g_context->pServer!=0);
	assert(g_context->pServer->port==5000);
	assert(strcmp(g_context->pServer->listen_ip,"localhost")==0);
	

	char * par3[3];
	
	par3[0]="./server_test";
	par3[1]="-f";
	par3[2]="../conf/server.conf";
	int ret=init_context(3,&par);
	assert(ret==0);
	assert(g_context->pServer!=0);
	assert(g_context->pServer->port==5000);
	assert(strcmp(g_context->pServer->listen_ip,"localhost")==0);
**/

	printf(" testInit_Context()  ok!\n");

}



void testServerRun(int argc,char ** argv)
{
	printf(" start==========testServerRun()\n");
	release();
	printf("prepare===================\n");
	init_context(argc,argv);	
	printf("finish==========test ServerRun ok\n");
}

void testInit_plugin()
{
	release();
	char * par[11];
	par[0]="./server_test";
	par[1]="-p";
	par[2]="6000";
	par[3]="-l";
	par[4]="192.168.1.1";
	par[5]="-m";
	par[6]="200";
	par[7]="-D";
	par[8]="log=wefwe";
	par[9]='\0';
	int ret=init_plugin(11,&par);
	assert(ret==0);

	ret=init_plugin(1,NULL);
	assert(ret==-1);

	ret=init_plugin(0,NULL);
	assert(ret==-1);
	printf(" testInit_plugin() ok\n");

}


void testReadFile()
{
	printf(" start==========testReadConfFile()\n");
	release();
	init();
	testReadConfFile("./../conf/server.conf");
	printf(" finish==========testReadConfFile() ok\n");
}


int main(int argc, char ** argv)
{
	if(*++argv=='\0')
		printf("sdfsdfs\n");
	

	setenv("LOG_LEVEL","3");
	testInit_Context();
	
	testInit_plugin();

	testReadFile();

	testServerRun(argc,argv);

}

