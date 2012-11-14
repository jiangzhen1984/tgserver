
#include <stdlib.h>
#include <stdio.h>
#include "tg_server.h"
#include "tg_context.h"
#include "tg_log.h"


static void printHelp()
{
		printf(" usage: ./tgserver -p -l  -m -D \n"
			"    -p port of number \n"
			"    -l addres of listening  \n"
			"    -m acceptable maxium clients \n"
			"    -D pass arg to service  name=arg \n"
			"    -o levle of log :0-ERROR 1-WARNING 2 INFO 3 DEBUG  \n"
			"    -h print this message \n"
			 );
}

int main(int argc,char ** argv)
{
	//set log level first
	setenv("LOG_LEVEL","2",1);
	char ** ppHead=argv;
	while(ppHead!=NULL && *ppHead!=NULL)
	{
		if(strcmp(*ppHead,"-h")==0)
		{
			printHelp();
			exit(0);
		}
		ppHead++;
	}
	int ret=init_context(argc,argv);
	logI("MAIN","================init context complete===============\n");
	if(ret==-1)
	{
		release();
		printHelp();
		exit(1);
			
	}
	
	init_plugin(argc,argv);
	logI("MAIN","================init plugins complete===============\n");

	startup();
	
	return 0;
}

