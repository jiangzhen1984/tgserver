
#ifndef TG_PROCESS_H
#define TG_PROCESS_H

#include "tg_context.h"
#include "tg_service.h"




// start to listen socket
// accept client connection and push connection to ppArClient
void startlisten(TGContext * pContext,TGClient ** ppArClient,struct TGService ** ppArService);


#ifdef DEBUG

void testListen();

void testThread();

#endif

#endif
