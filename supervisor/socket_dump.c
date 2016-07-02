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

pthread_mutex_t dump_mutex;


/* thread for dump application */
//void *sockDump(void *arg){
void *sockDump(OptionsProcess *options){

    int clientfd,sendbytes,recvbytes;
    struct hostent *host;
    struct sockaddr_in sockaddr;

    Machine *machine = &(options->process.machine[0]);

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
    genDump(packet_req, addrstr, options); // msg to be sent out
    logging(LOGFILE, "supervisor: 2\n");
    send(clientfd, packet_req, sizeof(Packet), MSG_NOSIGNAL);

    logging(LOGFILE, "supervisor: 3\n");
    packet_reply = (Packet *)malloc(sizeof(Packet));
    /* Receive neighbors_reply from remote side */
    Recv(clientfd, packet_reply, sizeof(Packet), MSG_NOSIGNAL);
    logging(LOGFILE, "supervisor: 4\n");


    /*got dump ack*/
    if(strcmp(packet_reply->packet_type, "0111") == 0) {

        logging(LOGFILE, "supervisor: 5\n");

    } // endof if(strcmp(packet_reply

    free(packet_reply);
    close(clientfd);
    //pthread_exit(0);
}

// start a client thread to register the services to register machine table
int dumpApp(OptionsProcess *options){
    char logmsg[128]; snprintf(logmsg, sizeof(logmsg), "(supervisor): socket client started, will dump the application.\n");
    logging(LOGFILE, logmsg); 

    options->process.machine[0].port = getRegisterPort(main.ip, REGISTER_MACHINE_FILE);
    options->process.machine[1].port = getRegisterPort(shadow.ip, REGISTER_MACHINE_FILE);

    // for main machine
    sockDump(options);
    return 0;
}
