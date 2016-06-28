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

/* thread for hello packet */
void *sockhello(void *arg){

    HelloArgs *helloArgs;
    helloArgs = (HelloArgs *)malloc(sizeof(HelloArgs));
    helloArgs = (HelloArgs *)arg;

    int clientfd;

    while(1){
        clientfd = Socket(AF_INET, SOCK_STREAM, 0);
        Connect(clientfd, helloArgs->sockaddr, sizeof(helloArgs->sockaddr));
        pthread_mutex_lock(&hello_mutex); // Critical section start
        //sendHello(clientfd, helloArgs->packet);
        pthread_mutex_unlock(&hello_mutex); // Critical section end
        
        sleep(APP_HEARTBEAT_INTERVAL); //sleep interval time
    } //endof while(1)

}

/* thread for manager registeration */
void *sockregister(void *arg){
    Machine *supervisor;
    supervisor = (char *)malloc(sizeof(Machine));
    supervisor = (Machine *)arg;

    int clientfd,sendbytes,recvbytes;
    struct hostent *host;
    struct sockaddr_in sockaddr;

    getaddr(hostname, addrstr); //get hostname and ip, getaddrinfo.h
   
    char logmsg[128]; snprintf(logmsg, sizeof(logmsg), "manager_client(0x%x): starting up, connecting to supervisor (%s)...\n", pthread_self(), supervisor->ip);
    logging(LOGFILE, logmsg);

    /* There are 3 types of Packets to be exchanged via manager for registering:
     * 1) Register service (from manager to register machine)         (000)
     * 2) Register acknowledge (from register machine to manager)     (001)
     * 3) Hello Packets (from manager to register machine)            (111)
     * */

    if((host = gethostbyname(supervisor->ip)) == NULL ) { // got the remote manager
        perror("gethostbyname");
        exit(-1);
    };

    /* connect to remote supervisor */
    /*create socket*/
    clientfd = Socket(AF_INET, SOCK_STREAM, 0);

    /*parameters for sockaddr_in*/
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(supervisor->port);
    sockaddr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(sockaddr.sin_zero), 8);

    /*connect to supervisor */
    Connect(clientfd,sockaddr,sizeof(sockaddr));


    logging(LOGFILE, "manager: 1\n");

    pthread_mutex_lock(&register_mutex);

    Packet *packet_req, *packet_reply;

    /* generate packet_req, provide:
     * - sender_ip (addrstr)
     * - supervisor_ip (remote_ipstr)*/
    packet_req = genRegister(addrstr, supervisor->ip); // msg to be sent out
    logging(LOGFILE, "manager: 2\n");
    send(clientfd, packet_req, sizeof(Packet), MSG_NOSIGNAL);

    logging(LOGFILE, "manager: 3\n");
    packet_reply = (Packet *)malloc(sizeof(Packet));
    /* Receive neighbors_reply from remote side */
    Recv(clientfd, packet_reply, sizeof(Packet), MSG_NOSIGNAL);
    logging(LOGFILE, "manager: 4\n");

    pthread_mutex_unlock(&register_mutex);

    /*packet_reply->Data.procedure_number should be packet_reply->Data.dup_numbers*/
    if(strcmp(packet_reply->packet_type, "001") == 0) {

        /*packet_reply->Data.dup_numbers = dup_number * 10 + manager_exists;*/
        //int manager_exists = (packet_reply->Data.procedure_number) % 10;
        //int dup_number = (packet_reply->Data.procedure_number) / 10;
        int manager_exists = 0;
        int dup_number = 0;

        /*check is duplicated or not*/
        if (dup_number == 0){
            //fprintf(stdout, "Congratulations, %d services rigistered successfully!\n",
            //       packet_req->Data.procedure_number);
        }
        //else if (dup_number == packet_req->Data.procedure_number){
        //    fprintf(stdout, "Duplicated rigisteration! Check your commands!\n");
        //}
        else{
            //fprintf(stdout, "Duplication found! %d of %d services rigistered successfully.\n",
            //        packet_req->Data.procedure_number - dup_number,
            //        packet_req->Data.procedure_number);
        }

    } // endof if(strcmp(packet_reply

    free(packet_reply);
    close(clientfd);
    pthread_exit(0);
}

// start a client thread to register the services to register machine table
int registerServices(Machine *machine){
    char logmsg[128];
    snprintf(logmsg, sizeof(logmsg), "(manager): socket client started, to be communicated with register machine.\n");
    logging(LOGFILE, logmsg); 

    pthread_t sockregisterid;
    pthread_create(&sockregisterid, NULL, &sockregister, (void *)machine);
    pthread_join(sockregisterid, NULL);

    return 0;
}
