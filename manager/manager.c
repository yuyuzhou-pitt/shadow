/* shadow Server, it plays two roles:
 *  1. as socket manager to provide services to client
 *  2. as socket client to register services into register machine table
 *
 * Steps to build and run:
 * $ make
 * $ ./manager
 * $ (manager)$ register <program-name> <version-number>
 * $ (manager)$ list
 *   Program Name | Version | Procedure | reg-or-not
*/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <ctype.h>

#define EXECUTE_SERV 1
#include "../packet/packet.h" //for Packet_Seq, use Execute_Serv to be the packet type

#include "socket_register.h"
#include "socket_manager.h"
#include "../lib/libsocket.h"
#include "../lib/getaddrinfo.h"
#include "../lib/liblog.h"
#include "../lib/libterminal.h"

#define PORT 0 //0 means assign a port randomly
#define BUFFER_SIZE  1024
#define NTHREADS 128

int main(int argc, char *argv[]){

    if(argc != 3 ){
        fprintf(stderr, "USAGE: ./manager <supervisor_ip> <supervisor_port>\n");
        exit(1);
    }

    Machine machine;
    snprintf(machine.ip, sizeof(machine.ip), "%s", argv[1]);
    machine.port = atoi(argv[2]);

    /* Thread attribute */
    pthread_attr_t attr;
    pthread_attr_init(&attr); // Creating thread attributes
    pthread_attr_setschedpolicy(&attr, SCHED_RR); // Round Robin scheduling for threads 
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // Don't want threads (particualrly main)

    /* start socket manager for client use */
    int sockfd;
    pthread_t sockmanagerid;
    pthread_create(&sockmanagerid, NULL, &sockmanager, (void *)sockfd);
    
    registerServices(&machine);
    pthread_join(sockmanagerid, NULL);
    return 0;
}
