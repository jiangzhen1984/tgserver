

#include "../tg_process_reader.h"
#include "../tg_process_response.h"
#include "../tg_context.h"
#include <stdlib.h>
#include <assert.h>

void test_read_data_from_cli()
{
	printf("#########test_read_data_from_cli() ######\n");

	// test case 1
	int r=read_data_from_cli(NULL,-1,NULL);
	assert(r==-1);
	printf("====finish test case 1=======\n");

	// test cat 2
	TGClient  pClient;
	memset(&pClient,0,sizeof(pClient));
	pClient.sockfd=10;
	int type;
	char * p=NULL;	
	r=read_data_from_cli(&pClient,&type,&p);
	assert(r==0);
	assert(type==MSG_TYPE_CLIENT_QUIT);
	assert(p==NULL);
	printf("====finish test case 2=======\n");

	// test case 3
	//  init client array first
	int fd[2];
	pipe(fd);
	pClient.sockfd=fd[0];
	type=-1;
	p=NULL;
	int nNum=0;
	char b[4];
	memcpy(b,&nNum,4);
	//write data	
	write(fd[1],b,4);
	r=read_data_from_cli(&pClient,&type,&p);
	assert(r==1);
	assert(type==MSG_TYPE_DATA_LENGTH_ERROR);
	assert(p==NULL);
	printf("====finish test case 3=======\n");

	
	// test case 4
	p=NULL;
	int len=16;		
	char buf[24];
	memcpy(buf,&len,4);
	memcpy(&buf[4],"tg://libdemo.so/",16);
	write(fd[1],buf,len+4);
	r=read_data_from_cli(&pClient,&type,&p);
	assert(r==0);
	assert(type==MSG_TYPE_CLIENT_REQUEST);
	printf("====%s\n",p);
	assert(strcmp(p,"tg://libdemo.so/")==0);
	printf("====finish test case 4=======\n");
	
	// test case 5
	p=NULL;
	int len1=296;
	char buf1[300];
	memset(buf1,0,sizeof(buf1));
	memcpy(buf1,&len1,4);
	int i=0;
	for(i=4;i<300;i++)
	{
		buf1[i]='A'+i;
	}
	int size=(unsigned char)buf1[3] << 24 | (unsigned char )buf1[2] << 16 | (unsigned char )buf1[1] << 8 | (unsigned char)buf1[0];
	printf("\n");

	printf("=====%d\n",size);
	write(fd[1],buf1,len1+4);
	r=read_data_from_cli(&pClient,&type,&p);
	assert(r==0);
	assert(type==MSG_TYPE_CLIENT_REQUEST);
	printf("p==:%s\n",p);
	printf("bu==:%s\n",&buf1[4]);
	assert(strcmp(p,&buf1[4])==0);
	printf("====finish test case 5=======\n");
	printf("#########test_read_data_from_cli() ok ######\n");
}

void test_read_data_from_main_process()
{
	printf("#########test_read_data_from_main_process() ######\n");

	// test case 1
	int r=read_data_from_main_process(NULL,-1,NULL);
	assert(r==-1);
	printf("====finish test case 1=======\n");

	// test cat 2
	int fd[2];
	pipe(fd);
	TGClient  pClient;
	memset(&pClient,0,sizeof(pClient));
	pClient.readPipe=fd[0];
	close(fd[0]);
	close(fd[1]);
	int type;
	char * p=NULL;	
	r=read_data_from_main_process(&pClient,&type,&p);
	assert(r==-1);
	printf("====finish test case 2=======\n");

	// test case 3
	pipe(fd);
	p=NULL;
	type=-1;
	memset(&pClient,0,sizeof(pClient));
	pClient.readPipe=fd[0];
	write(fd[1],"tg://0932/",10);
	r=read_data_from_main_process(&pClient,&type,&p);
	assert(r==1);
	printf("====finish test case 3=======\n");
	
	// test case 4
	type=1;
	p=NULL;	
	write(fd[1],"tg://1030/",10);
	r=read_data_from_main_process(&pClient,&type,&p);
	assert(r==0);

	printf("#########test_read_data_from_main_process() ok######\n");
}


int main(int argc, char ** argv)
{
	test_read_data_from_cli();

	test_read_data_from_main_process();
}
