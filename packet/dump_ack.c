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

Packet *genDumpAck(Packet *packet_recv){
    /*wrap into packet*/
    Packet *_packet;
    _packet = (Packet *)malloc(sizeof(Packet)); //Packet with Register_Machineice type Data

    snprintf(_packet->sender_ip, sizeof(_packet->sender_ip), "%s", packet_recv->receiver_ip);
    snprintf(_packet->receiver_ip, sizeof(_packet->receiver_ip), "%s", packet_recv->sender_ip);
    snprintf(_packet->packet_type, sizeof(_packet->packet_type), "%s", "0111");

    _packet->Data.app_process = (AppProcess) packet_recv->Data.app_process; // Data

    /*checksum*/
    snprintf(_packet->PacketChecksum, sizeof(_packet->PacketChecksum), 
             "%d", chksum_crc32((unsigned char*) _packet, sizeof(*_packet)));

    return _packet;
}
        
int sendDumpAck(int sockfd, Packet *packet_recv){
   /* generate neighbors_reply reply according to configure file */
   Packet *packet_reply;
   packet_reply = (Packet *)malloc(sizeof(Packet)); //Packet with Register_Service type Data

   packet_reply = genDumpAck(packet_recv); // msg to be sent back
   return Send(sockfd, packet_reply, sizeof(Packet), MSG_NOSIGNAL);
}
