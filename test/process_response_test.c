

#include "../tg_process_response.h"
#include <assert.h>



void testHandleResponse()
{

//handle_response(TGClient *pTGCli,struct TGEvent *pEv);
}




void testHandleResponseClientRequest()
{
	int fd[2];
	pipe(fd);
	TGClient cli;
	cli.sockfd=fd[1];
	int ret=handle_response_client_request(&cli,NULL);
	assert(ret==-1);
	printf("-----\n");

	struct TGEvent event;
	ret=handle_response_client_request(&cli,&event);
	printf("00000\n");
	assert(ret==-1);
	char buf[255];
	memset(buf,0,sizeof(buf));
	read(fd[0],buf,sizeof(buf));
	assert(strcmp(buf,"tg://2003/")==0);
	printf(" test HandleResponseClientRequest() ok !\n");
}


void testHandleResponseDataLengthError()
{
	int fd[2];
	pipe(fd);
	TGClient cli;
	cli.sockfd=fd[1];
	int ret=handle_response_data_length_error(&cli,NULL);
	assert(ret==0);

	char buf[255];
	memset(buf,0,sizeof(buf));
	read(fd[0],buf,sizeof(buf));
	assert(strcmp(buf,"tg://2001/")==0);
	printf(" test HandleResponseDataLengthError() ok !\n");
	
}



int main(int argc ,char ** argv)
{
	setenv("LOG_LEVEL","3");
	testHandleResponseDataLengthError();

	testHandleResponseClientRequest();
}
