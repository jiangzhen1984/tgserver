
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>


int main(int argc,char ** argv)
{
	while(1)
	{
	struct sockaddr_in sockaddr;
	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		printf(" create sock error\n");
	}

	sockaddr.sin_family=AF_INET;
	sockaddr.sin_port=htons(6666);
	sockaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	int ret=connect(sock,&sockaddr,sizeof(struct sockaddr));
	if(ret==-1)
	{
		printf("===connect socket error\n");
		exit(1);
	}

	char * s="tg://libdemo2.so/all";
	int size=strlen(s);
	int r=write(sock,&size,sizeof(size));
	r=write(sock,s,size);
	close(sock);
	}

}
