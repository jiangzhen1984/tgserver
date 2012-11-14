
#ifndef TG_SERVER_H
#define TG_SERVER_H



// init server context according to stratup parameters
// -p port -l listen-path -m max-clients -s service dir or path -s service dir or path
// -o loglevel(0-ERROR 1-WARNING 2-INFO 3-DEBUG)
// -Dservicename(library name)=service paramers
// -f parameters-file-path the file rule is following:
//	port=	
//	listen=
//	max-clients=
//	log-level=
//	[D]service-name=paramters
// Note: the service paramters will pass to create and init functions
int init_context(int argc, char** argv);


// init plugins of server
// the service parameters is passed by startup paramters of prefix with -D
// -Dservicename(library name)=service paramers
// or [D]service-name=paramters in file
int init_plugin(int argc,char ** argv);


//start to listen
void startup();


#ifdef DEBUG
	void testReadConfFile(const char * path);
#endif

#endif

