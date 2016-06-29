#ifndef __LIB_TERMINAL__
#define __LIB_TERMINAL__

#include "../packet/packet.h"

/*for terminal options*/
typedef struct OptionsStruct{
    int count; //the total number of the options
    char command[128]; //. i.e.: register
    char option1[128]; //i.e.: program name
    char option2[128]; //i.e.: version number
    char option3[128]; //i.e.: procedure name
    char option4[1024]; //i.e.: input file, will be term ("term1 term2") if searching
    char option5[1024]; //i.e.: output file
    char option6[1024]; //i.e.: output file
    char action[30]; // split, index, or search, will also be the temp result directory
    char term[128]; // terms, e.g.: "term1"
    char remote_ipstr[100];
    char remote_port[6];
    int manager_no;  // for sort use
    int manager_number;  // for sort use
}OptionsStruct;

/*for launch / leap command*/
typedef struct OptionsProcess{
    int index; // which machine, master or slave
    char command[128];
    AppProcess process;
}OptionsProcess;

/*split the string to array*/
typedef struct SplitStr{
    int count;
    char terms[255][30]; // can store 255 terms totally
}SplitStr;

/*************
* 1. general *
*************/

OptionsStruct *command2struct(char *input);
OptionsProcess *struct2process(OptionsStruct *option_struct);
int quit();

/*****************
* 2. supervisor *
*****************/

int listPortMapper();
int helpPortMapper();

/************
* 3. manager *
************/

int helpServer();

/************
* 4. client *
************/
int helpClient();

#endif
