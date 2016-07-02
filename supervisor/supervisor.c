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
    
    char command[128];
    char command_no_n[128];
    char *sub_command; // sub-command and options
    struct OptionsStruct *options;
    options = (struct OptionsStruct *)malloc(sizeof(struct OptionsStruct));
    struct OptionsProcess *process;
    //options = (struct OptionsProcess *)malloc(sizeof(struct OptionsProcess));

    int terminal = 1;
    while(terminal == 1){
        //fprintf(stdout, "(supervisor: %s)# ", router->router_id);
        fprintf(stdout, "(supervisor)# ");
	fflush(stdin);
	if (fgets(command, sizeof(command), stdin) != NULL ){
            snprintf(command_no_n, strlen(command), "%s", command);
            options = command2struct(command_no_n);
            //OptionsStruct optionStruct =
            sub_command = options->command;

            if(strcmp(sub_command, "quit") == 0){ // there is a \n in the stdin
                /* TODO: clean threads before quit terminal */
                if((terminal = quit()) == 0){
                    break;
                }
            }
            else if(strcmp(sub_command, "help") == 0){ // there is a \n in the stdin
                helpPortMapper();
            }
            else if(strcmp(sub_command, "leap") == 0){ // there is a \n in the stdin

                if(options->count < 3){
                    helpPortMapper();
                }
                else{
                    process = struct2process(options);
                    leapApp(process);
                }
            }
            else if(strcmp(sub_command, "launch") == 0){ // there is a \n in the stdin

                if(options->count < 4){
                    helpPortMapper();
                }
                else{
                    process = struct2process(options);
                    launchApp(process);
                }
            }
            else if(strcmp(sub_command, "list") == 0){ // there is a \n in the stdin
                listPortMapper();
            }
            else if(strcmp(sub_command, "(null)") != 0){ // there is a \n in the stdin
                fprintf(stderr, "ERROR: command not found: %s\n", sub_command);
                helpPortMapper();
            }

        }
        else{
            fprintf(stderr, "Not all fields are assigned\n");
        }

    }

    unlinkUpperFile(addrstr);
    //pthread_join(sockmanagerid, NULL);
    return 0;
}
