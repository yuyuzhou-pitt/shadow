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

pthread_mutex_t register_mutex;
pthread_mutex_t hello_mutex;


/* thread for launch application */
void *sockLaunch(void *arg){
    OptionsProcess *options;
    options = (char *)malloc(sizeof(struct OptionsStruct));
    options = (struct OptionsStruct *)arg;

    Machine *manager = &(options->process.machine[options->index]);

    int clientfd,sendbytes,recvbytes;
    struct hostent *host;
    struct sockaddr_in sockaddr;

    getaddr(hostname, addrstr); //get hostname and ip, getaddrinfo.h
   
    char logmsg[128]; snprintf(logmsg, sizeof(logmsg), "supervisor_client(0x%x): starting up, connecting to manager (%s)...\n", pthread_self(), manager->ip);
    logging(LOGFILE, logmsg);

    if((host = gethostbyname(manager->ip)) == NULL ) { // got the remote machine
        perror("gethostbyname");
        exit(-1);
    };

    /* connect to remote supervisor */
    /*create socket*/
    clientfd = Socket(AF_INET, SOCK_STREAM, 0);

    /*parameters for sockaddr_in*/
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(manager->port);
    sockaddr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(sockaddr.sin_zero), 8);

    /*connect to supervisor */
    Connect(clientfd,sockaddr,sizeof(sockaddr));


    logging(LOGFILE, "supervisor: 1\n");

    pthread_mutex_lock(&register_mutex);

    Packet *packet_req, *packet_reply;
    packet_req = (Packet *)malloc(sizeof(Packet));

    /* generate packet_req, provide:
     * - sender_ip (addrstr)
     * - supervisor_ip (remote_ipstr)*/
    genLaunch(packet_req, addrstr, options); // msg to be sent out
    logging(LOGFILE, "supervisor: 2\n");
    send(clientfd, packet_req, sizeof(Packet), MSG_NOSIGNAL);

    logging(LOGFILE, "supervisor: 3\n");
    packet_reply = (Packet *)malloc(sizeof(Packet));
    /* Receive neighbors_reply from remote side */
    Recv(clientfd, packet_reply, sizeof(Packet), MSG_NOSIGNAL);
    logging(LOGFILE, "supervisor: 4\n");

    pthread_mutex_unlock(&register_mutex);

    /*packet_reply->Data.procedure_number should be packet_reply->Data.dup_numbers*/
    if(strcmp(packet_reply->packet_type, "0101") == 0) {

        logging(LOGFILE, "supervisor: 5\n");

    } // endof if(strcmp(packet_reply

    free(packet_reply);
    close(clientfd);
    pthread_exit(0);
}

// start a client thread to register the services to register machine table
int launchApp(OptionsProcess *options){
    char logmsg[128];
    snprintf(logmsg, sizeof(logmsg), "(supervisor): socket client started, will launch the application.\n");
    logging(LOGFILE, logmsg); 

    pthread_t sockid_main, sockid_shadow;
    // for main machine
    options->process.machine[options->index].port = getRegisterPort(options->process.machine[options->index].ip, REGISTER_MACHINE_FILE);
    pthread_create(&sockid_main, NULL, &sockLaunch, (void *)options);
    // for shadow machine
    options->index = 1;
    options->process.machine[options->index].port = getRegisterPort(options->process.machine[options->index].ip, REGISTER_MACHINE_FILE);
    pthread_create(&sockid_shadow, NULL, &sockLaunch, (void *)options);

    pthread_join(sockid_main, NULL);
    pthread_join(sockid_shadow, NULL);
    return 0;
}
