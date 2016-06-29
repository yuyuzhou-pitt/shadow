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

Packet *genLaunch(char *supervisor_ip, OptionsProcess *options){
    /*wrap into packet*/
    Packet *_packet;
    _packet = (Packet *)malloc(sizeof(Packet)); //Packet with Register_Machineice type Data

    snprintf(_packet->sender_ip, sizeof(_packet->sender_ip), "%s", supervisor_ip);
    snprintf(_packet->receiver_ip, sizeof(_packet->receiver_ip), "%s", options->process.machine[options->index]);
    snprintf(_packet->packet_type, sizeof(_packet->packet_type), "%s", "0100");

    _packet->Data.app_process = (AppProcess) options->process; // Data

    /*checksum*/
    snprintf(_packet->PacketChecksum, sizeof(_packet->PacketChecksum), 
             "%d", chksum_crc32((unsigned char*) _packet, sizeof(*_packet)));

    return _packet;
}
