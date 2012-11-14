
#ifndef DEMO3_H
#define DEMO3_H


extern "C"
{

void * create(char * pPar);


int init(void * pPar);


void * do_request(void * pPar);


int release(void * pPar);


void destroy(void * pPar);


}

#endif
