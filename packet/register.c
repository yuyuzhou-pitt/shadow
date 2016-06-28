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

Packet *genRegister(char *manager_ip, char *supervisor_ip){
    AppProcess app_process; // MUST NOT be pointer as to be send remote
    snprintf(app_process.main.ip, sizeof(app_process.main.ip), "%s", manager_ip);
    app_process.main.port = getPortNumber(manager_ip);
    app_process.main.status = 0; // default is 0 (running)
    app_process.main.latest_heartbeat = time(0); // system time

    /*wrap into packet*/
    Packet *register_packet;
    register_packet = (Packet *)malloc(sizeof(Packet)); //Packet with Register_Machineice type Data

    snprintf(register_packet->sender_ip, sizeof(register_packet->sender_ip), "%s", manager_ip);
    snprintf(register_packet->receiver_ip, sizeof(register_packet->receiver_ip), "%s", supervisor_ip);
    snprintf(register_packet->packet_type, sizeof(register_packet->packet_type), "%s", "000"); // Register Packets (000)

    register_packet->Data.app_process = (AppProcess) app_process; // Data

    /*checksum*/
    snprintf(register_packet->PacketChecksum, sizeof(register_packet->PacketChecksum), 
             "%d", chksum_crc32((unsigned char*) register_packet, sizeof(*register_packet)));

    return register_packet;
}
