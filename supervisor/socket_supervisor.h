#ifndef __SOCKET_PORT_MAPPER_H__
#define __SOCKET_PORT_MAPPER_H__

//#ifndef MSG_NOSIGNAL
//#define MSG_NOSIGNAL 0
//#endif

//#include "../packet/linkload.h"

void *supervisor_thread(void *arg);
void *sock_supervisor(void *arg);

#endif
