
#ifndef TG_CONTEXT_H
#define TG_CONTEXT_H

#include "tg_service.h"

#define DEFAULT_MAX_CLIENTS 20
#define DEFAULT_LISTEN_PORT 6666 

typedef struct{
	int port;
	char * listen_ip;
	char * name;
	int version;
	int maxClients;
} TGServer;

typedef struct{
	TGServer * pServer;
	int logLevel;
	int curClientCounts;
	int listenfd;
	//The descriptor is uesd to local administration communication
	int localAdminfd;
} TGContext;


#ifdef LINUX
#include <pthread.h>
typedef pid_t tg_pid;
#elif WIN32
typedef int  tg_pid;
#endif


typedef struct{
	tg_pid pid;
	int sockfd;
	int readPipe;	
	int writePipe;
	char * clientIP;
	int   port;
} TGClient;


extern TGClient **  g_clients;
extern TGContext * g_context;
extern struct TGService ** g_services;


void init();
void release();


void initClient(TGClient ** ppClient);


#define FIND_CLIENT_SUCCESS 0
#define FIND_CLIENT_NOT_FOUND -1
// find client in g_clients array
// according to process id. if find it, set pointer to ppClient.
// return 0 SUCCESS and find it
// reutrn -1 not found client 
int findClient(const tg_pid pid, TGClient ** ppClient);


// add client to global clients array(g_clients)
// it will copy client data
void addClient(TGClient * pClient);


// close client connection
//  and remove client from  global clients array
void closeClient(const tg_pid pid);

// release client memory
// close client all sock descriptior
void destroyClient(TGClient * pCli);


//close all client
void closeAllClient();

//get activate client counts
int getCurrentClientCounts();


// get current connected client
// and return every client one line
// Note: after use data, must free ppClientInfo data
//  pid   ip  port
void getCurrentClientsInfo(char ** ppCliInfo);


// send signal to client
// return 0 send successfully
// return -1 send error client pipe closed
int sendSignalToClient(TGClient * pCli,char * msg);

// send signal to all activite clients
// return 0 send successfully
// return >0 that failed client counts
int sendSignalToAllClients(char * msg);





#endif
