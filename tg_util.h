#ifndef TG_UTIL_H
#define TG_UTIL_H


// find  sepeator
int findSeparator(char * source,char * sepor);

int lastfindSeparator(char * source,char *sepor);


void getLibName(char * path,char**pName);


// get current process id
int getTGServerPid();

#endif
