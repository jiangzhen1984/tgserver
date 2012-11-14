
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

struct line{
	int x;
	int y;
	int endx;
	int endy;
};

struct point{
	int x;
	int y;
};

int main(int argc,char ** argv)
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

	char * s="tg://libdemo.so/all";
	int size=strlen(s);
	int r=write(sock,&size,sizeof(size));
	r=write(sock,s,size);
	printf("after write===========%d\n",r);

	char buf[500];
	memset(buf,0,sizeof(buf));
	int n=read(sock,buf,sizeof(buf));
	printf("===========%d\n",n);
	if(n>4)
	{
		int p=0;
		for(p=0;p<n;p++)
		{
			printf("%02x",(unsigned char)buf[p]);
		}
		printf("\n");
		int rSize=(unsigned char)buf[3]>>24 |(unsigned char)buf[2] >>16| (unsigned char )buf[1] >> 8 | (unsigned char )buf[0];
		printf("-----get size:   %d  actual:%d\n",rSize,n);
		if(rSize>n)
		{
			read(sock,&buf[n],rSize-n);
		}
		char header[11];
		memset(header,0,sizeof(header));
		memcpy(header,&buf[4],sizeof(header)-1);
		printf("===get header:%s\n",header);

		int pointSize=buf[17]>> 24 | buf[16]>> 16  | buf[15] >> 8 | buf[14];
		printf("==get point size:%d\n",pointSize);
		int i=0;
		for(i=0;i<pointSize;i++)	
		{
			void * ph=&buf[i*sizeof(struct point)+18];
			struct line * pl=(struct line *) malloc(sizeof(struct point));
			memset(pl,0,sizeof(struct point));
			memcpy(pl,ph,sizeof(struct point));
			printf("=====get point: x:%d y:%d\n",pl->x,pl->y);
			free(pl);
		}	

		
	}
	else
	{
		printf("======error\n");
	}

	while(1)
	{
		sleep(10);
	}
	close(sock);
	sleep(1);

}
