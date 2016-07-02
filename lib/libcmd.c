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

    //systemLaunch(cmd);
    #define WAIT_FOR_COMPLETION
    systemLaunch(cmd);
}

int systemRestore(Packet *packet_recv){
    char cmd[255];
    memset(cmd, 0, sizeof(cmd));
    // scritp
    strcpy(cmd, "../application/script/restore-app.sh ");
    strcat(cmd, packet_recv->Data.app_process.pid);
    // target directory
    systemLaunch(cmd);
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

#ifdef WAIT_FOR_COMPLETION
    timeout = 10000;

    while (0 == waitpid(my_pid , &status , WNOHANG)) {
            if ( --timeout < 0 ) {
                    perror("timeout");
                    return -1;
            }
            sleep(1);
    }

    //printf("%s WEXITSTATUS %d WIFEXITED %d [status %d]\n",
    //        argv[0], WEXITSTATUS(status), WIFEXITED(status), status);

    if (1 != WIFEXITED(status) || 0 != WEXITSTATUS(status)) {
            perror("%s failed, halt system");
            return -1;
    }

#endif

    return 0;
}
