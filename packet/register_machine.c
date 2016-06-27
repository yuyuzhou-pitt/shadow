#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#define REGISTER_MACHINE 1
#include "packet.h"

#include "checksum.h"
#include "../lib/liblog.h"
#include "../lib/libfile.h"
#include "../lib/libterminal.h"


/* Format for register machine table:
 *   Server IP  | Sever Port | Program Name | Version | Procedure
 *   192.168.1,1| 54321      | ScientificLibrary | 1  | Multiply
*/

int writeRegisterMachine(Packet *packet_recv, char *file){
    char registerString[128];
    AppProcess *app_process;
    app_process = (AppProcess *)malloc(sizeof(app_process));

    char hostfile[17];
    memset(hostfile, 0, sizeof(hostfile));

    strcpy(hostfile, "../");
    strcat(hostfile, file);

    int file_exists = 1;
    if(access(hostfile, F_OK) < 0) {
        char logmsg[128]; snprintf(logmsg, sizeof(logmsg), "readfile: File not found: %s\n", hostfile);
        logging(LOGFILE, logmsg);
        file_exists = 0;
    }

    char *tempfile = ".tempfile";

    if (file_exists == 1){
        if(copy_file(hostfile, tempfile) != 0)
            fprintf(stderr, "Error during copy!");
    }

    int manager_exists = 0; //for sending hello if it's a new manager
    int dup_number = 0; //record the duplication numbers
    int i;
    //for(i=0;i<packet_recv->Data.procedure_number;i++){
        /*printable str*/
        memset(registerString, 0, sizeof(registerString));
        snprintf(registerString, sizeof(registerString), "%s:%s\n", \
             packet_recv->sender_ip, packet_recv->Data.app_process.main.port);

        /*check duplication before writing*/

        FILE *table_fp;
        char *line = NULL;
        size_t len = 0;
        ssize_t read;

        /*do not check dup if file do not exists*/
        if(file_exists == 1) {
            if ((table_fp = fopen(tempfile,"r")) < 0){
                char logmsg[128]; snprintf(logmsg, sizeof(logmsg), "readfile: Failed to open file: %s\n", tempfile);
                logging(LOGFILE, logmsg);
                return -1;
            }

            while ((read = getline(&line, &len, table_fp)) != -1) {
                char *manager_ip, *manager_port;
                manager_ip = strtok_r(line, ":", &manager_port);

                /*to find the register info which match the request program_name, version_number, and procedure_name*/
                if(strcmp(manager_ip, packet_recv->Data.app_process.main.ip) == 0 &&
                   strcmp(manager_port, packet_recv->Data.app_process.main.port) == 0){
                    server_exists = 1;
                    break; //jump out the while loop
                }
            }
            if (line) free(line);
            fclose(table_fp);
        }

        if (server_exists == 1){
            printf("Duplicate registration: %s", registerString);
            dup_number ++;
            //continue;
        }

        if(writeFile(registerString, strlen(registerString), hostfile, "a") < 0){
            perror("writeport");
            return -1;
        }

        char logmsg[128]; snprintf(logmsg, sizeof(logmsg), "write register into register machine table. \n");
        logging(LOGFILE, logmsg);

    //}

    unlinkFile(tempfile);
    return manager_exists;
}
