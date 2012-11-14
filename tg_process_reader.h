
#ifndef TG_PROCESS_READER_H
#define TG_PROCESS_READER_H
#include "tg_context.h"

// read data from pClient
// return 0 handle successfully
// return -1 read data  failed 
// return 1 read data successfully but content that client sent is error 
// pData is used to malloc function to allocate
// and isn't necessary free because deamon process thread will be free
int read_data_from_cli(TGClient * pClient,int *pType, char**ppData);



// read data from main process pipe
// return 0 handle successfully
// return -1 read data  failed 
// return 1 read data successfully but content that client sent is error 
// ppData is used to malloc function to allocate
int read_data_from_main_process(TGClient *pClient,int *pType, char** ppData);

#endif
