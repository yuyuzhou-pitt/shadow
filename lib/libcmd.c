#define _XOPEN_SOURCE 500
#define __USE_BSD
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <pthread.h>
#include "../packet/packet.h"
#include "../lib/liblog.h"

void *systemThread(void *arg){
    char *cmd = (char *)arg;

    logging(LOGFILE, cmd);
    if (-1 == system(cmd)) {
        perror("child process execve failed [%m]");
        return -1;
    }

    pthread_exit(0);
}

int systemDump(Packet *packet_recv, int sockfd){
    /* execution */
    char cmd[255];
    memset(cmd, 0, sizeof(cmd));
    // scritp
    strcpy(cmd, "/bin/bash ../application/script/dump-app.sh ");
    // application
    strcat(cmd, packet_recv->Data.app_process.app.options);
    strcat(cmd, " ");
    // target machine (shadow)
    strcat(cmd, packet_recv->Data.app_process.machine[1].ip);
    strcat(cmd, ":/tmp/");
    //strcat(cmd, packet_recv->Data.app_process.machine[0].ip);

    //systemLaunch(cmd);
    systemLaunch(cmd, sockfd);
}

int systemRestore(Packet *packet_recv, int sockfd){
    char cmd[255];
    memset(cmd, 0, sizeof(cmd));
    // scritp
    strcpy(cmd, "/bin/bash ../application/script/restore-app.sh ");
    // application
    strcat(cmd, packet_recv->Data.app_process.app.options);
    strcat(cmd, " ");
    // app pid
    char pid[32]; snprintf(pid, 10, "%d", packet_recv->Data.app_process.pid);
    strcat(cmd, pid);
    // target directory
    systemLaunch(cmd, sockfd);

    /* Thread attribute */
    //pthread_t tid;
    //pthread_create(&tid, NULL, &systemThread, (void *)cmd);
}

int systemKill(Packet *packet_recv, int sockfd){
    char cmd[255];
    memset(cmd, 0, sizeof(cmd));
    // scritp
    strcpy(cmd, "/bin/bash ../application/script/kill-app.sh ");
    // application
    strcat(cmd, packet_recv->Data.app_process.app.options);
    strcat(cmd, " ");
    // app pid
    char pid[32]; snprintf(pid, 10, "%d", packet_recv->Data.app_process.pid);
    strcat(cmd, pid);
    // target directory
    systemLaunch(cmd, sockfd);

    /* Thread attribute */
    //pthread_t tid;
    //pthread_create(&tid, NULL, &systemThread, (void *)cmd);
}

/*http://stackoverflow.com/questions/5237482/how-do-i-execute-external-program-within-c-code-in-linux-with-arguments*/
int systemLaunch(char *cmd, int sockfd)
{
    pid_t   my_pid, my_sid;
    int     status, timeout /* unused ifdef WAIT_FOR_COMPLETION */;

    if (0 == (my_pid = fork())) {

    /* Cancel certain signals */
    signal(SIGCHLD,SIG_DFL); /* A child process dies */
    signal(SIGTSTP,SIG_IGN); /* Various TTY signals */
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGHUP, SIG_IGN); /* Ignore hangup signal */
    signal(SIGTERM,SIG_DFL); /* Die on SIGTERM */

    /* Change the file mode mask */
    umask(0);

    /* Create a new SID for the child process */
    my_sid = setsid();

            close(sockfd);
            logging(LOGFILE, cmd);
            logging(LOGFILE, ".\n");
            if (-1 == system(cmd)) {
                    perror("child process execve failed [%m]");
                    return -1;
            }
    }

    return 0;
}
