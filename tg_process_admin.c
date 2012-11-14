


#include "tg_process_admin.h"
#include "tg_context.h"



// The function is used to 
// admin server
// pReq:
//   tg://1040/ display all clients  write clients data to fd descriptor 
//   tg://1041/pid shutdown client accroding to pid : no data write to descriptor of fd
void write_data(int fd,char * pReq)
{
	if(pReq==NULL || *pReq=='\0')
	{
		return;
	}
	//TODO check fd is available
	
	if(strncmp(pReq,"tg://1040/",10)==0)
	{
		char * pData=NULL;
		getCurrentClientsInfo(&pData);
		if(pData!=NULL)
		{
			write(fd,pData,strlen(pData));
			write(fd,"\n\n",2);
		}
		else
		{
			write(fd,"none\n",4);
		}
		
	}
	else if(strncmp(pReq,"tg://1041/",10)==0)
	{
		int id=atoi(&pReq[10]);
		if(id>0)
		{
			TGClient *pCli;
			if(findClient(id,&pCli)==0 &&pCli!=NULL)
			{
				// write message to client pipe
				write(pCli->writePipe,"tg://1005",10);	
				return;
			}	
		}
		
		// write error message	
		char buf[100];
		memset(buf,0,sizeof(buf));
		sprintf(buf,"unkown pid:%d\n",id);	
		write(fd,buf,strlen(buf));
	
	}

}

