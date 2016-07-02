#define _XOPEN_SOURCE 500
#define __USE_BSD
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include "../packet/packet.h"

int systemDump(Packet *packet_recv){
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

    // execute
    system(cmd);
}

int systemRestore(Packet *packet_recv){
    char cmd[17];
    memset(cmd, 0, sizeof(cmd));
    // scritp
    strcpy(cmd, "../application/script/restore-app.sh ");
    strcat(cmd, "/tmp/");
    strcat(cmd, packet_recv->Data.app_process.machine[0].ip);
    // target directory
    system(cmd);
}

/*http://stackoverflow.com/questions/5237482/how-do-i-execute-external-program-within-c-code-in-linux-with-arguments*/
int systemLaunch(char *cmd)
{
    pid_t   my_pid;
    int     status, timeout /* unused ifdef WAIT_FOR_COMPLETION */;

    if (0 == (my_pid = fork())) {
            if (-1 == system(cmd)) {
                    perror("child process execve failed [%m]");
                    return -1;
            }
    }

    return 0;
}
