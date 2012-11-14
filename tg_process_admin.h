
#ifndef TG_PROCESS_ADMIN_H
#define TG_PROCESS_ADMIN_H


// The function is used to 
// admin server
// pReq:
//   tg://1040/ display all clients  write clients data to fd descriptor 
//   tg://1041/pid shutdown client accroding to pid : no data write to descriptor of fd
void write_data(int fd,char * pReq);

#endif
