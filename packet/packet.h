#ifndef __PACKET_H__
#define __PACKET_H__

#include <stdint.h>


/* Define the heartbeat pakcet interval in seconds */
#define MACHINE_HEARTBEAT_INTERVAL 20
#define APP_HEARTBEAT_INTERVAL 10

/* AppFile identification, path to exectue app*/
typedef struct AppPath{
     char name[80];
     char location[255];
     char location[255];
}AppPath;


/* Machine list table maintained by supervisor */
typedef struct MachineList{




}


/* global application list */
typedef struct ApplicationList{



}



/* All message will be wrapped into packet */
typedef struct Packet{
    char sender_ip[32];
    char recevier_ip[32];

    char packet_type[4];
 
    #ifdef REGISTER_MACHINE
        Register_Machine Data;
    #elif REGISTER_MACHINE_ACK
        Register_Machine_Ack Data;
    #elif MACHINE_HEARTBEAT
        Machine_Heartbeat Data;
    #elif LAUNCH_APP
        Launch_App Data;
    #elif LAUNCH_APP_ACK
        Launch_App_Ack Data;
    #elif APP_HEARTBEAT
        App_Heartbeat Data;
    #elif DUMP_APP
        Dump_App Datat;
    #elif DUMP_APP_ACK
        Dump_App_Ack Data; 
    #elif LEAP_APP
        Leap_App Data;
    #elif LEAP_APP_ACK
        Leap_App_ACK Data;
    #else
        Unknown Data;
    #endif 
} Packet;

#endif
