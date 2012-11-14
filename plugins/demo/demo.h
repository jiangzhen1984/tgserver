
#ifndef DEMO_H
#define DEMO_H



void * create(char * pPar);


int init(void * pPar);


void * do_request(void * pPar);


int release(void * pPar);


void destroy(void * pPar);

#endif
