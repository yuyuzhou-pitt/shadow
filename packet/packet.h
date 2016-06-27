#ifndef __PACKET_H__
#define __PACKET_H__

#include <stdint.h>
 
/*define the internal hidden file*/
#define SUPERVISOR_FILE ".supervisor"
#define REGISTER_MACHINE_FILE ".register_machine"

/* Define the heartbeat pakcet interval in seconds */
#define MACHINE_HEARTBEAT_INTERVAL 20
#define APP_HEARTBEAT_INTERVAL 10

/* AppFile identification, path to exectue app*/
typedef struct AppPath{
    char name[80];
    char location[255];
}AppPath;


/* Machine table maintained by supervisor*/
typedef struct Machine{
    char ip[32];
    int port;
    int status; // running, stopped
    time_t latest_heartbeat;
}Machine;

/* global application process table*/
typedef struct AppProcess{
    AppPath app_path;
    Machine main;
    Machine shadow;
}AppProcess;


/*register machine to supervisor*/
typedef struct Register_Machine{
    char packet_type[5]; //0000
    char version_number[8]; //why it's char?
    Machine machine;
}Register_Machine;

typedef struct Register_Machine_Ack{
    char packet_type[5]; //0001
    char version_number[8]; //why it's char?
    Machine machine;
    int dup_numbers; //the program name on managers
}Register_Machine_Ack;

typedef struct Machine_Heartbeat{
    char packet_type[5]; //0010
    char version_number[8]; //why it's char?
    Machine machine;
    time_t date_time;
}MachineHeartbeat;

typedef struct Launch_App{
    char packet_type[5]; //0011
    char version_number[8]; //why it's char?
    AppProcess app_process;
}LaunchApp;

typedef struct Launch_App_Ack{
    char packet_type[5]; //0100
    char version_number[8]; //why it's char?
    AppProcess app_process;
}LaunchAppAck;

typedef struct App_Heartbeat{
    char packet_type[5]; //0101
    char version_number[8]; //why it's char?
    AppProcess app_process;
}AppHeartbeat;

typedef struct Dump_App{
    char packet_type[5]; //0110
    char version_number[8]; //why it's char?
    AppProcess app_process;
}DumpApp;

typedef struct Dump_App_Ack{
    char packet_type[5]; //0111
    char version_number[8]; //why it's char?
    AppProcess app_process;
}DumpAppAck;

typedef struct Leap_App{
    char packet_type[5]; //1000
    char version_number[8]; //why it's char?
    AppProcess app_process;
}LeapApp;

typedef struct Leap_App_Ack{
    char packet_type[5]; //1001
    char version_number[8]; //why it's char?
    AppProcess app_process;
}LeapAppAck;

typedef struct Unknown{
    char packet_type[5]; //1010
    char version_number[8]; //why it's char?
    AppProcess app_process;
}Unknown;

/* All message will be wrapped into packet */
typedef struct Packet{
    char sender_ip[32];
    char receiver_ip[32];

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
    char PacketChecksum[32]; // crc32
}Packet;

#endif
