/* Single-thread version datagram socket.
 *  - assign port automatically
 *  - timeout after given time
 *  - accept multi-client connection
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>

#include "socket_manager.h"
#define REGISTER_MACHINE 1
#include "../packet/packet.h"
#include "../packet/register.h"
#include "../lib/libsocket.h"
#include "../lib/getaddrinfo.h"
#include "../lib/liblog.h"
#include "../lib/libfile.h"

#define PORT 0 //0 means assign a port randomly
#define BUFFER_SIZE  1024
#define MAX_QUE_CONN_NM 1024
#define TIMER 3600  //Timeout value in seconds
#define CONTINUE 1 
#define NTHREADS 5
#define WINDOW_SIZE 4

char hostname[1024]; // local hostname and domain
char addrstr[100]; // local ip address (eth0)
int port; // local socket port

pthread_t threadid[NTHREADS]; // Thread pool

struct thread_info{
    pthread_t thread_id;  /* ID returned by pthread_create()*/
    int thread_num;       /* Application-defined thread #*/
    char *argv_string;    /* From command-line argument */
};

pthread_mutex_t mutex;

//Packet_Seq *executePacketSeq; //the execute send sequence
//Packet_Seq *executeResultSeq; //the execute reply sequence

/*thread for packet handle, ack scheme*/
//void *manager_thread(int sockfd, struct sockaddr_in manager_sockaddr){
void *manager_thread(void *arg){
    int sockfd = (int *)arg;

    struct sockaddr_in client_sockaddr;

    OptionsStruct *result_options;
    result_options = (OptionsStruct *)malloc(sizeof(OptionsStruct));
    char client_ip[100];
    char manager_ip[100];

    char logmsg[128];
    int receiveFlag = 1;
    int sendFlag = 1; 
    int counter = 0;

    //Packet_Seq *incoming_head, *outgoing_head, *trimed_head;
    //incoming_head = initListSeq();
    //Packet_Seq *outgoing_head;
    //outgoing_head = (Packet_Seq *)malloc(sizeof(Packet_Seq));
    //outgoing_head = initListSeq();
    //trimed_head = incoming_head; //trimed header is used to avoid unessary link list scan. 

    /*lock the send back in case it interrupt by other threads*/
    pthread_mutex_lock(&mutex);

    //Packet_Seq *result_seq, *send_seq;
    Packet *packet_recv, *packet_reply, *packet_ack; // MUST use pointer to fit different Packet
    packet_recv  = (Packet *)calloc(1, sizeof(Packet));
    packet_ack = (Packet *)calloc(1, sizeof(Packet));
    //packet_recv = (Packet *)calloc(1, sizeof(Packet));
 
    logging(LOGFILE, "==manager_thread 1==\n");
    //Recvfrom(sockfd, &packet_recv, sizeof(Packet), MSG_NOSIGNAL, (struct sockaddr *)&client_sockaddr, sizeof(client_sockaddr));
    Recv(sockfd, packet_recv, sizeof(Packet), MSG_NOSIGNAL);
    //recvExecuteServ(sockfd, (struct sockaddr *)&client_sockaddr, &packet_recv);
    logging(LOGFILE, "==manager_thread 2==\n");

    //printf("Start communicating with Client %s.\n\n", packet_recv.sender_ip);

    char trans_id[32]; //record the transaction id, in case multiple sequences
    Packet *execute_ack;// = genExecuteAck(trimed_head);
    int endTransaction = 0;
    if(strcmp(packet_recv->packet_type, "0100") == 0){ //execute packet
        logging(LOGFILE, "==manager_thread 3==\n");
        sendLaunchAck(sockfd, &packet_recv);
    }

    pthread_mutex_unlock(&mutex);

    free(result_options);
    //shutdown(sockfd, SHUT_RDWR);
    //snprintf(logmsg, sizeof(logmsg), "managerthread(0x%x): served request, exiting thread\n", pthread_self());
    //logging(LOGFILE, logmsg);
    //pthread_exit(0);
}

void *sockmanager(void *arg){
    /*initial the global seq to store execute packet from client*/

    struct sockaddr_in manager_sockaddr, client_sockaddr;
    int sin_size, recvbytes, sendbytes;
    int sockfd, client_fd, desc_ready;
    char logmsg[128];

    sin_size=sizeof(manager_sockaddr);

    /* Data structure to handle timeout */
    struct timeval before, timer, *tvptr;
    struct timezone tzp;
    
    /* Data structure for the select I/O */
    fd_set ready_set, test_set;
    int maxfd, nready, client[FD_SETSIZE];

    /* create socket */
    //sockfd = Socket(AF_INET,SOCK_DGRAM,0);
    sockfd = Socket(AF_INET,SOCK_STREAM,0); //use TCP

    /* set parameters for sockaddr_in */
    manager_sockaddr.sin_family = AF_INET;
    manager_sockaddr.sin_port = htons(PORT); //0, assign port automatically in 1024 ~ 65535
    manager_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY); //0, got local IP automatically
    bzero(&(manager_sockaddr.sin_zero), 8);
   
    int i = 1;//enable reuse the combination of local address and socket
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));
    Bind(sockfd, manager_sockaddr);

    getaddr(hostname, addrstr); //get hostname and ip, getaddrinfo.h
    port = Getsockname(sockfd, manager_sockaddr, sin_size);  /* Get the port number assigned*/
    writePort(port, addrstr);
   
    snprintf(logmsg, sizeof(logmsg), "sockmanager: Server %s (%s) is setup on port: %d\n", addrstr, hostname, port);
    logging(LOGFILE, logmsg);
    Listen(sockfd, MAX_QUE_CONN_NM);
   
    /* Thread attribute */
    pthread_attr_t attr;
    pthread_attr_init(&attr); // Creating thread attributes
    pthread_attr_setschedpolicy(&attr, SCHED_RR); // Round Robin scheduling for threads 
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // When a thread is created detached, its thread ID and other resources can be reused as soon as the thread terminates.
    int iThread = 0; // Thread iterator

    /* Set up the I/O for the socket, select mode */
    int k;
    for(k=0;k<FD_SETSIZE;k++){
        client[k] = -1;
    }
  
    FD_ZERO(&ready_set);
    FD_ZERO(&test_set);
    FD_SET(sockfd, &test_set);
    maxfd = sockfd;

    /* Initialize the timeval struct to TIMER seconds */
    timer.tv_sec = TIMER;
    timer.tv_usec = 0;
    tvptr = &timer;

    /* Set up the time out by getting the time of the day from the system */
    gettimeofday(&before, &tzp);

    int status;
    status=CONTINUE;
    while (status==CONTINUE){
        if (iThread == NTHREADS){
            iThread = 0;
        }

        memcpy(&ready_set, &test_set, sizeof(test_set));
        nready = select(maxfd+1, &ready_set, NULL, NULL, tvptr);

        switch(nready){
            case -1:
                fprintf(stderr, "sockmanager: errno: %d.\n", errno);
                perror("\nSELECT: unexpected error occured.\n");
                logging(LOGFILE, "\nSELECT: unexpected error occured.\n");

                /* remove bad fd */
                for(k=0;k<FD_SETSIZE;k++){
                    if(client[k] > 0){
                        struct stat tStat;
                        if (-1 == fstat(client[k], &tStat)){
                            fprintf(stderr, "fstat %d error:%s", sockfd, strerror(errno));
                            FD_CLR(client[k], &ready_set);
                        }
                    }
                }
                status=-1;
                break;
            case 0:
                /* timeout occuired */
                fprintf(stderr, "sockmanager: TIMEOUT... %d.\n", errno);
                status=-1;
                break;
            default:
                if (FD_ISSET(sockfd, &ready_set)){

                    //pthread_create(&threadid[iThread], &attr, &manager_thread, (void *)threadSocket);
                    //pthread_join(threadid[iThread], NULL);
                    //iThread++;
                    //manager_thread(sockfd, manager_sockaddr);



                    client_fd = Accept(sockfd, client_sockaddr, sin_size);
                    logging(LOGFILE, "supervisor: 4\n");
                    for(k=0;k<FD_SETSIZE;k++){
                        if(client[k] < 0){
                            client[k] = client_fd;
                        }
                    }

                    FD_SET(client_fd, &test_set);
                    if (client_fd > maxfd) maxfd = client_fd;
                    snprintf(logmsg, sizeof(logmsg), "sockmanager(0x%x): Descriptor %d is readable\n",  pthread_self(), client_fd);
                    logging(LOGFILE, logmsg);
                    pthread_create(&threadid[iThread], &attr, &manager_thread, (void *)client_fd);
                    pthread_join(threadid[iThread], NULL);
                    iThread++;
                }// end if (FD_ISSET(i, &ready_set))
        }// end switch
    } // end while (status==CONTINUE)
    close(sockfd);
    unlinkPortFile(addrstr);

    return 0;
}
