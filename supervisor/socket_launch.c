/* As socket client:
 * 1) client starts and run into busy wait
 * 2) to get supervisor IP:Port for SUPERVISOR_FILE
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#define REGISTER_MACHINE 1
#include "../packet/packet.h"
#include "../packet/register.h"
#include "../lib/libsocket.h"
#include "../lib/getaddrinfo.h"
#include "../lib/liblog.h"
#include "../lib/libfile.h"
#include "../lib/libterminal.h"

#define PORT   0 //4321
#define BUFFER_SIZE 1024
#define READ_PORT_INTERVAL 20 // busy wait interval to read port files


char hostname[1024]; // local hostname and domain
char addrstr[100]; // local ip address (eth0)

typedef struct HelloArgs{
    struct sockaddr_in sockaddr;
    Packet *packet;
}HelloArgs;

pthread_mutex_t launch_mutex;
pthread_mutex_t hello_mutex;


/* thread for launch application */
//void *sockLaunch(void *arg){
void *sockLaunch(Machine *machine, AppPath *app){

    int clientfd,sendbytes,recvbytes;
    struct hostent *host;
    struct sockaddr_in sockaddr;

    getaddr(hostname, addrstr); //get hostname and ip, getaddrinfo.h
   
    char logmsg[128]; snprintf(logmsg, sizeof(logmsg), "supervisor_client(0x%x): starting up, connecting to manager (%s)...\n", pthread_self(), machine->ip);
    logging(LOGFILE, logmsg);

    if((host = gethostbyname(machine->ip)) == NULL ) { // got the remote machine
        perror("gethostbyname");
        exit(-1);
    };

    /* connect to remote supervisor */
    /*create socket*/
    clientfd = Socket(AF_INET, SOCK_STREAM, 0);

    /*parameters for sockaddr_in*/
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(machine->port);
    sockaddr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(sockaddr.sin_zero), 8);

    /*connect to supervisor */
    Connect(clientfd,sockaddr,sizeof(sockaddr));


    logging(LOGFILE, "supervisor: 1\n");


    Packet *packet_req, *packet_reply;
    packet_req = (Packet *)malloc(sizeof(Packet));

    /* generate packet_req, provide:
     * - sender_ip (addrstr)
     * - supervisor_ip (remote_ipstr)*/
    genLaunch(packet_req, addrstr, machine, app); // msg to be sent out
    logging(LOGFILE, "supervisor: 2\n");
    send(clientfd, packet_req, sizeof(Packet), MSG_NOSIGNAL);

    logging(LOGFILE, "supervisor: 3\n");
    packet_reply = (Packet *)malloc(sizeof(Packet));
    /* Receive neighbors_reply from remote side */
    Recv(clientfd, packet_reply, sizeof(Packet), MSG_NOSIGNAL);
    logging(LOGFILE, "supervisor: 4\n");


    /*packet_reply->Data.procedure_number should be packet_reply->Data.dup_numbers*/
    if(strcmp(packet_reply->packet_type, "0101") == 0) {

        logging(LOGFILE, "supervisor: 5\n");

    } // endof if(strcmp(packet_reply

    free(packet_reply);
    close(clientfd);
    //pthread_exit(0);
}

// start a client thread to register the services to register machine table
int launchApp(OptionsProcess *options){
    char logmsg[128]; snprintf(logmsg, sizeof(logmsg), "(supervisor): socket client started, will launch the application.\n");
    logging(LOGFILE, logmsg); 

    Machine main = options->process.machine[0];
    main.port = getRegisterPort(main.ip, REGISTER_MACHINE_FILE);
    Machine shadow = options->process.machine[1];
    shadow.port = getRegisterPort(shadow.ip, REGISTER_MACHINE_FILE);
    
    // for main machine
    sockLaunch(&main, &(options->process.app));

    // for shadow machine
    sockLaunch(&shadow, &(options->process.app));
    return 0;
}
