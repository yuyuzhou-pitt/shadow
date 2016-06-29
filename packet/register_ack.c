#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "checksum.h"
#define REGISTER_MACHINE_ACK 1
#include "packet.h"

#include "../lib/liblog.h"
#include "../lib/libsocket.h"

Packet *genRegisterReply(Packet *register_req, int dup_numbers){
    //int getPort(char *portstr, int size, char *ipfile)
    Register_Machine_Ack register_ack; // MUST NOT be pointer as to be send remote
    //snprintf(register_ack.program_name, sizeof(register_ack.program_name), "%s", register_req->Data.program_name);
    //snprintf(register_ack.version_number, sizeof(register_ack.version_number), "%s", register_req->Data.version_number);
    //register_ack.dup_numbers = dup_numbers;

    /*wrap into packet*/
    Packet *register_reply;
    register_reply = (Packet *)malloc(sizeof(Packet)); //Packet with Register_Service type Data

    snprintf(register_reply->sender_ip, sizeof(register_reply->sender_ip), "%s", register_req->receiver_ip);
    snprintf(register_reply->receiver_ip, sizeof(register_reply->receiver_ip), "%s", register_req->sender_ip);
    snprintf(register_reply->packet_type, sizeof(register_reply->packet_type), "%s", "0001"); // Register Ack (001)

    //register_reply->Data = (Register_Machine_Ack) register_ack; // Data

    /*checksum*/
    //snprintf(register_reply->PacketChecksum, sizeof(register_reply->PacketChecksum), "%d", chksum_crc32((unsigned char*) register_reply, sizeof(*register_reply)));

    return register_reply;
}

// sendRegisterReply(sockfd, packet_reply);
int sendRegisterReply(int sockfd, Packet *packet_req, int dup_numbers){
   /* generate neighbors_reply reply according to configure file */
   Packet *packet_reply;
   packet_reply = (Packet *)malloc(sizeof(Packet)); //Packet with Register_Service type Data

   packet_reply = genRegisterReply(packet_req, dup_numbers); // msg to be sent back
   return Send(sockfd, packet_reply, sizeof(Packet), MSG_NOSIGNAL);
}
