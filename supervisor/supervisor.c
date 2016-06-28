/* SRPC Port-Mapper, the bridge between RPC Server and Client
 *  1. Port Mapper receives registration from Server and stores it into Port 
 *     Mapper Table. The format of register machine Table looks like below:
 *       ServerIP | ServerPort | ProgramName | Version | Procedure
 *  2. Port Mapper also responses for the request from Client request for the
 *     services information in Port Mapper Table.
 *
 * Steps to build and run:
 * $ make
 * $ ./supervisor
 * $ (supervisor)$ list
 *   Server IP  | Sever Port | Program Name | Version | Procedure
 *   192.168.1,1| 54321      | ScientificLibrary | 1  | Multiply
 *   192.168.1,1| 54321      | ScientificLibrary | 1  | Sort
 *   192.168.1,1| 54321      | ScientificLibrary | 1  | Min
 *   192.168.1,1| 54321      | ScientificLibrary | 1  | Max
 *   192.168.1,2| 54322      | ScientificLibrary | 1  | Multiply
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include "socket_supervisor.h"
#include "../lib/getaddrinfo.h"
#include "../lib/libsocket.h"
#include "../lib/libterminal.h"
#include "../lib/liblog.h"
#include "../lib/libfile.h"

#define PORT 0 //0 means assign a port randomly
#define BUFFER_SIZE  1024
#define NTHREADS 128

//RemoteProgram *(*libraryPtr)();

int main(int argc, char *argv[]){

    if(argc > 1){
        fprintf(stderr, "USAGE: ./supervisor (no options required)\n");
        exit(1);
    }

    //libraryPtr = getLibraryPtr(); // configurable library function

    /*initial the load balance link for client request service use*/

    char hostname[1024]; // local hostname and domain
    char addrstr[100]; // local ip address (eth0)
    getaddr(hostname, addrstr);

    /* Thread attribute */
    pthread_attr_t attr;
    pthread_attr_init(&attr); // Creating thread attributes
    pthread_attr_setschedpolicy(&attr, SCHED_RR); // Round Robin scheduling for threads 
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // Don't want threads (particualrly main)

    /* start register machine socket thread */
    int sockfd = 0;
    pthread_t sockmanagerid; 
    pthread_create(&sockmanagerid, NULL, &sock_supervisor, (void *)sockfd);
    fprintf(stdout, "(supervisor): Staring supervisor...\n");
    while( getPortNumber(addrstr) == -1 );
    fprintf(stdout, "(supervisor): Supervisor started (ip port): %s %d.\n", addrstr, getPortNumber(addrstr));

    fprintf(stdout, "(supervisor): please track log file for detail: %s\n", LOGFILE);
    fprintf(stdout, "\n== WELCOME TO TERMINAL FOR SHADOW! ==\n");
    fprintf(stdout, "\nEnter the commands 'help' for usage.\n\n");
    
    char command[64];
    int terminal = 1;
    char *split1, *split2, *strsplit;
    while(terminal == 1){
        //fprintf(stdout, "(supervisor: %s)# ", router->router_id);
        fprintf(stdout, "(supervisor)# ");
	fflush(stdin);
	if (fgets(command, sizeof(command), stdin) != NULL ){
            strsplit = command;
            split1 = strtok_r(strsplit, " ", &split2);

            if(strcmp(split1, "quit\n") == 0){ // there is a \n in the stdin
                /* TODO: clean threads before quit terminal */
                if((terminal = quit()) == 0){
                    break;
                }
            }
            else if(strcmp(split1, "help\n") == 0){ // there is a \n in the stdin
                helpPortMapper();
            }
            else if(strcmp(split1, "list\n") == 0){ // there is a \n in the stdin
                listPortMapper();
            }
            else if(strcmp(split1, "\n") != 0){ // there is a \n in the stdin
                fprintf(stderr, "ERROR: command not found: %s\n", split1);
                helpPortMapper();
            }

        }
        else{
            fprintf(stderr, "Not all fields are assigned\n");
        }

    }

    //unlinkUpperFile(SUPERVISOR_FILE);
    //pthread_join(sockmanagerid, NULL);
    return 0;
}
