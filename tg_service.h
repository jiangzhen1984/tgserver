

#ifndef TG_SERVICE_H
#define TG_SERVICE_H


#ifndef DEFAULT_SERVICE_SIZE
#define DEFAULT_SERVICE_SIZE 10
#endif

#define SERVICE_DEFAULT_PARA_COUNTS 5

#define SERVICE_CREATE_FUNCTION_NAME "create"
#define SERVICE_INIT_FUNCTION_NAME "init"
#define SERVICE_DO_REQUEST_FUNCTION_NAME "do_request"
#define SERVICE_RELEASE_FUNCTION_NAME "release"
#define SERVICE_DESTROY_FUNCTION_NAME "destroy"

#define DEFAULT_CONF_PATH "conf/server.conf"
#define DEFAULT_SERVICE_DIR "plugins/"



// This function return global variable like share memory pointer
// all clients processes access it
// And struct TGService pGlobalData will hold this returned variable
typedef void * (*SERVICE_CREATE_FUNCTION)(char *);

// just initialize for client
// when client connection is established, then this function will be call
typedef int (*SERVICE_INIT_FUNCTION)(void *);

// when client send request , this function will be called 
// And returned data will be send to client
// return parameter restrict:
// |____4 bytes as data length without self length___|_____ data segment____|
// The whole returned parameter length is 4 + data length
// if returned parameter is NULL, it means don't need to send data to client
// Note: the returned parameter must allocate with malloc
// when after send data , it will use free for releaseing memory
typedef void * (*SERVICE_DO_REQUEST_FUNCTION)(void *);

// when client connection is destroyed, then this function will be call
typedef int (*SERVICE_RELEASE_FUNCTION)(void *);

// when server shut down this function will be call
// The struct TGService variable of pGlobalData as paramter 
typedef void (*SERVICE_DESTROY_FUNCTION)(void *);


struct TGService{
	int sID;
	char * serviceName;
	char ** parameters;
	int parCounts;
	SERVICE_CREATE_FUNCTION do_create;
 	SERVICE_INIT_FUNCTION  do_init;
	SERVICE_DO_REQUEST_FUNCTION do_request;
	SERVICE_RELEASE_FUNCTION do_release;
	SERVICE_DESTROY_FUNCTION do_destroy;
	// this variable holds point to function of do_create returned
	void * pGlobalData;
};

extern struct TGService ** g_services;

// load library from path
// and save funtion address to struct TGService
void loadService(const char * path);


//unload all services from global list
void unLoadService();


// find struct TGService according to id
// and return new TGService if found.
// return NULL if not found
// Note: The returnd service must be free after use
struct TGService * findServiceI(const int id);


// find struct TGService according to id
// and return new TGService 
// return NULL if not found
// Note: The returnd service must be free after use
struct TGService * findServiceN(const char * name);

// push par to service
// Note: this function must invoke before do_create function
void setServiceCreateParametersByN(const char * name,const char * par);

// push par to service
// Note: this function must invoke before do_create function
void setServiceCreateParameters(struct TGService * pSer,const char * par);


// when client connection is established
// this function is called by client process
// Note: this function never is called in main process
void initAllServices();

// when client connection is closed
// this function is called by client process
// Note: this function never is called in main process
void releaseAllServices();

// when server is startingup 
// server call this function for initialize all service 
void createAllServices();

// when server is shuting down
// server call this function for destroing all service
void destroyAllServices();


#endif

