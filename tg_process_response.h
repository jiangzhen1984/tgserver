
#ifndef TG_PROCESS_RESPONSE_H
#define TG_PROCESS_RESPONSE_H

#include "tg_context.h"
#include "tg_service.h"
#include "tg_event.h"

// define maco fot that response event function
// use which function to handle events
#define MSG_TYPE_BASE 1000
#define MSG_TYPE_CLIENT_QUIT 1000
#define MSG_TYPE_CLIENT_REQUEST 1001
#define MSG_TYPE_DATA_LENGTH_ERROR 1002
#define MSG_TYPE_SERVER_SHUT_DOWN 1003
#define MSG_TYPE_CLIENT_INIT 1004
#define MSG_TYPE_CLIENT_BE_CLOSED_BY_ADMIN 1005

//-------------the following is used to admin server
// list all current clients that connect server
// send tg://1040/ > /tmp/tgp
#define MSG_TYPE_LIST_ACTIVATE_CLIENTS 1040
// close client connection by administrator
// send tg://1041/pid > /tmp/tgp 
#define MSG_TYPE_CLOSE_CLIENT 1041
#define MSG_TYPE_MAX 1050



// define maco for that send this NO. to client
// to tell client result
#define RESPONSE_BASE 2000
#define RESPONSE_SEND_DATA 2000
#define RESPONSE_DATA_LENGTH_ERROR 2001
#define RESPONSE_REQUIRE_STR_ILLEGAL 2002
#define RESPONSE_SERVER_SHUT_DOWN 2003
#define RESPONSE_REQUIRE_SERVICE_NOT_FOUND 2004
#define RESPONSE_MAX_CONNECTION 2005
#define RESPONSE_RETURNED_DATA_FORMAT_ILLEGAL 2006
#define RESPONSE_CONN_CLOSED_BY_ADMIN 2007
#define RESPONSE_MAX 2050


// define function pointer that handle response 
// all function return 0 as success
// all function return 1 as error
typedef int (*RESPONSE_FUNCTION)(TGClient *pTGcli,struct TGEvent * pEv);

// handle response event function 
// if TGEvent parameter is NULL or TGEvent->type is more than MSG_TYPE_MAX
// return -1
// otherwise return function returned
int handle_response(TGClient *pTGCli,struct TGEvent *pEv);

// handle response event for client quit
int handle_response_client_quit(TGClient *pTGcli,struct TGEvent * pEv);

// handle response event for client request
int handle_response_client_request(TGClient *pTGcli,struct TGEvent * pEv);

// handle response event for that client sended data length error
int handle_response_data_length_error(TGClient *pTGcli,struct TGEvent * pEv);

// handle response event for server shut down
int handle_response_server_shut_down(TGClient *pTGcli,struct TGEvent * pEv);

// handle response event for that client connection is established
int handle_response_client_init(TGClient *pTGcli,struct TGEvent * pEv);

// handle response event for that client connection was closed by administrator
int handle_response_client_conn_closed_by_admin(TGClient *pTGcli,struct TGEvent * pEv);





#endif
