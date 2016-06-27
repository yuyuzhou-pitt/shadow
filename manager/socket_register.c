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
    struct OptionsStruct *options;
    options = (struct OptionsStruct *)malloc(sizeof(struct OptionsStruct));
    options = (struct OptionsStruct *) arg;

    int clientfd,sendbytes,recvbytes;
    struct hostent *host;
    struct sockaddr_in sockaddr;

    getaddr(hostname, addrstr); //get hostname and ip, getaddrinfo.h
   
    /* try to get manager hostname and port from the host files (.<managername>) */
    char remote_manager[32];
    char remote_ipstr[32];
    char remote_portstr[6]; // to store the port

    memset(remote_ipstr, 0, sizeof(remote_ipstr));
    memset(remote_portstr, 0, sizeof(remote_portstr));

    char logmsg[128];
    snprintf(logmsg, sizeof(logmsg), "manager_lient(0x%x): busy wait for register machine starting up...\n", pthread_self());
    logging(LOGFILE, logmsg);

    int portFound = 0; // check port file exists or not
    while(1){
        memset(remote_portstr, 0, sizeof(remote_portstr)); 
        /* Steps:
         * 1) to find port file .<supervisor-ip> for supervisor
         * 2) if port file do NOT exists, busy wait
         * 3) if port file exists, communicate supervisor through this port
         */

        /* 1) go through all the direct_link_addr in cfg file */
        char templine[32];

        int iget;
        /* 2) if port file do NOT exists, busy wait */
        if((iget = getSupervisor(remote_portstr, remote_ipstr, SUPERVISOR_FILE)) < 0 ){ // read port file

            sleep(READ_PORT_INTERVAL);
            printf("client(0x%x): No available port found, make sure register machine started.\n", pthread_self());
            printf("client(0x%x): wait %d seconds to try again..\n", pthread_self(), READ_PORT_INTERVAL);

            //continue; // if file does not exists, continue
        }
        /* 3) if port file exists, communicate supervisor through this port*/
        else if(strlen(remote_portstr) == 5){
            portFound = 1;
            break;
        }
    }

    /* There are 3 types of Packets to be exchanged via manager for registering:
     * 1) Register service (from manager to register machine)         (000)
     * 2) Register acknowledge (from register machine to manager)     (001)
     * 3) Hello Packets (from manager to register machine)            (111)
     * */

    if(portFound == 1){
        if((host = gethostbyname(remote_ipstr)) == NULL ) { // got the remote manager
            perror("gethostbyname");
            exit(-1);
        };

        /* connect to remote manager */
        /*create socket*/
        clientfd = Socket(AF_INET, SOCK_STREAM, 0);

        /*parameters for sockaddr_in*/
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_port = htons(atoi(remote_portstr));
        sockaddr.sin_addr = *((struct in_addr *)host->h_addr);
        bzero(&(sockaddr.sin_zero), 8);

        /*connect to manager*/
        Connect(clientfd,sockaddr,sizeof(sockaddr));

    }

    pthread_mutex_lock(&register_mutex);

    Packet *packet_req, *packet_reply;

    /* generate packet_req, provide:
     * - program_name, version_number (options)
     * - sender_ip (addrstr)
     * - supervisor_ip (remote_ipstr)*/
    packet_req = genRegister(options, addrstr, remote_ipstr); // msg to be sent out
    send(clientfd, packet_req, sizeof(Packet), MSG_NOSIGNAL);

    packet_reply = (Packet *)malloc(sizeof(Packet));
    /* Receive neighbors_reply from remote side */
    Recv(clientfd, packet_reply, sizeof(Packet), MSG_NOSIGNAL);

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
    free(options);
    close(clientfd);
    pthread_exit(0);
}

// start a client thread to register the services to register machine table
int registerServices(struct OptionsStruct *options){
    char logmsg[128];
    snprintf(logmsg, sizeof(logmsg), "(manager): socket client started, to be communicated with register machine.\n");
    logging(LOGFILE, logmsg); 

    pthread_t sockregisterid;
    pthread_create(&sockregisterid, NULL, &sockregister, (void **)options);
    pthread_join(sockregisterid, NULL);

    return 0;
}
