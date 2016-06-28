#ifndef __SOCKET_REGISTER_H__
#define __SOCKET_REGISTER_H__


//#include "../supervisor/socket_supervisor.h"
#include "../lib/libterminal.h"

int registerServices(Machine *machine);
void *sockclient(void *arg);

#endif
