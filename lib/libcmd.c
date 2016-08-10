#define _XOPEN_SOURCE 500
#define __USE_BSD
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include "../packet/packet.h"

int systemDump(Packet *packet_recv, int sockfd){
    /* execution */
    char cmd[255];
    memset(cmd, 0, sizeof(cmd));
    // scritp
    strcpy(cmd, "../application/script/dump-app.sh ");
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
    strcpy(cmd, "../application/script/restore-app.sh ");
    char pid[32]; snprintf(pid, 10, "%d", packet_recv->Data.app_process.pid);
    strcat(cmd, pid);
    // target directory
    systemLaunch(cmd, sockfd);
}

/*http://stackoverflow.com/questions/5237482/how-do-i-execute-external-program-within-c-code-in-linux-with-arguments*/
int systemLaunch(char *cmd, int sockfd)
{
    pid_t   my_pid;
    int     status, timeout /* unused ifdef WAIT_FOR_COMPLETION */;

    if (0 == (my_pid = fork())) {
            close(sockfd);
            if (-1 == system(cmd)) {
                    perror("child process execve failed [%m]");
                    return -1;
            }
    }

    return 0;
}
