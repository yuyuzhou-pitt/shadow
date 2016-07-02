/* This file provides commands for the shadow terminal.
 * It divides into four parts:
 * 1. general
 * 2. supervisor
 * 3. manager
 * 4. client
 */

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "liblog.h"
#include "../packet/packet.h"
#include "../lib/libterminal.h"

/*************
* 1. general *
*************/

// parse command into options, 3 the most
// # launch main_ip shawdow_ip app parameters
// # leap main_ip shadow_ip app parameters
OptionsStruct *command2struct(char *input){
    size_t count = 0;
    char *tmp = input;
    char *last_delim;
   /* Count how many elements will be extracted. */
    while (*tmp){
        if (' ' == *tmp){
            count++;
            last_delim = tmp;
        }
        tmp++;
    }
    /* Add space for trailing token. */
    count += last_delim < (input + strlen(input) - 1);

    /*split the input string*/
    char *command, *argument1, *argument2, *argument3, *argument4;
    char *temp1, *temp2, *temp3; 
    OptionsStruct *parse_options;
    parse_options = (OptionsStruct *)calloc(1, sizeof(OptionsStruct));
    command = strtok_r(input, " ", &temp1); //option2 is for temp use
    argument1 = strtok_r(temp1, " ", &temp2);
    argument2 = strtok_r(temp2, " ", &temp3);
    argument3 = strtok_r(temp3, " ", &argument4);
    parse_options->count = count;
    snprintf(parse_options->command, sizeof(parse_options->command), "%s", command);
    snprintf(parse_options->option1, sizeof(parse_options->option1), "%s", argument1);
    snprintf(parse_options->option2, sizeof(parse_options->option2), "%s", argument2);
    snprintf(parse_options->option3, sizeof(parse_options->option3), "%s", argument3);
    snprintf(parse_options->option4, sizeof(parse_options->option4), "%s", temp3); // pass full shell cmd to execute
    return parse_options;
}

// convert struct to process
OptionsProcess *struct2process(OptionsStruct *option_struct){

    OptionsProcess *opt_pro;
    opt_pro = (OptionsProcess *)calloc(1, sizeof(OptionsProcess));
    opt_pro->index = 0; // default is master machine

    snprintf(opt_pro->command, sizeof(opt_pro->command), "%s", option_struct->command);
    snprintf(opt_pro->process.machine[0].ip, sizeof(opt_pro->process.machine[0].ip), "%s", option_struct->option1);
    snprintf(opt_pro->process.machine[1].ip, sizeof(opt_pro->process.machine[1].ip), "%s", option_struct->option2);
    snprintf(opt_pro->process.app.name, sizeof(opt_pro->process.app.name), "%s", option_struct->option3);
    snprintf(opt_pro->process.app.options, sizeof(opt_pro->process.app.options), "%s", option_struct->option4);
    return opt_pro;
}

OptionsStruct *argv2struct(int argc, char *argv[]){
    OptionsStruct *parse_options;
    parse_options = (OptionsStruct *)calloc(1, sizeof(OptionsStruct));
    snprintf(parse_options->command, sizeof(parse_options->command), "%s", argv[1]);
    //fprintf(stderr, "parse_options->command=%s.\n", parse_options->command);
    if (argc > 2){
        snprintf(parse_options->option1, sizeof(parse_options->option1), "%s", argv[2]);
        snprintf(parse_options->option2, sizeof(parse_options->option2), "%s", argv[3]);
        snprintf(parse_options->option3, sizeof(parse_options->option3), "%s", argv[4]);
        //fprintf(stderr, "parse_options->option1=%s.\n", parse_options->option1);
        //fprintf(stderr, "parse_options->option2=%s.\n", parse_options->option2);
        //fprintf(stderr, "parse_options->option3=%s.\n", parse_options->option3);
    }
    if (argc > 5){
        snprintf(parse_options->option4, sizeof(parse_options->option4), "%s", argv[5]);
        snprintf(parse_options->option5, sizeof(parse_options->option5), "%s", argv[6]);
        //fprintf(stderr, "parse_options->option4=%s.\n", parse_options->option4);
        //fprintf(stderr, "parse_options->option5=%s.\n", parse_options->option5);
    }
    if (argc > 7){
        snprintf(parse_options->option6, sizeof(parse_options->option6), "%s", argv[7]);
    }
    return parse_options;
}

int *str2array(SplitStr *result, char *a_str, const char a_delim){
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp){
        if (a_delim == *tmp){
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    //count++;

    //SplitStr *result;
    //result = (SplitStr *)malloc(sizeof(SplitStr));

    result->count = count;

    size_t idx  = 0;
    char* token = strtok(a_str, delim);

    int iN = 0;
    while (token){
        assert(idx < count);
        snprintf(result->terms[iN], sizeof(result->terms[iN]), strdup(token));
        token = strtok(0, delim);
        iN ++;
    }
    if(idx == count - 1){
        snprintf(result->terms[0], sizeof(result->terms[0]), a_str);
    }

    return 0;
    //return result;
}

// parse command into options, 3 the most
int command2array(char *command, char options[3][32]){
    char *option1, *option2, *option3, *option4;
    option1 = strtok_r(command, " ", &option2); //option2 is for temp use
    option3 = strtok_r(option2, " ", &option4);
    strncpy(options[0], option1, 32);
    strncpy(options[1], option3, 32);
    strncpy(options[2], option4, 32);
    //options[0] = option1;
    //options[1] = option3;
    //options[2] = option4;
    return 0;
}

// exit the services
int quit(){
    char confirm[4];
    fprintf(stderr, "WARNING: Do you want to quit? Please confirm: [y/N]");
    scanf("%c", confirm);
    if(confirm[0] == 'y'){
    //if(strcmp(confirm, "y") == 0 || strcmp(confirm, "y\273AJ4") == 0 ) { //confirm[0] == 'y'){
        return 0;
    }
    return 1;
}

/*****************
* 2. supervisor *
*****************/

int helpPortMapper(){
    fprintf(stderr, "\nUsage: <subcommand>\n");
    fprintf(stderr, "A user interface for the shadow system.\n\n");
    fprintf(stderr, "Subcommands:\n");
    fprintf(stderr, "  help      show this message\n");
    fprintf(stderr, "  list      list the current services in register machine table\n");
    fprintf(stderr, "  launch    <main-ip> <shadow-ip> <app> <options>\n");
    fprintf(stderr, "            launch the application on master and shadow machine\n");
    fprintf(stderr, "                <main-ip>   main ip \n");
    fprintf(stderr, "                <shadow-ip> shadow ip\n");
    fprintf(stderr, "                <app>       application to be running\n");
    fprintf(stderr, "                <options>   options\n");
    fprintf(stderr, "  quit      stop shadow socket and quit\n");
    return 0;
}

/*for supervisor terminal, to print the requst*/
int listPortMapper(){
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    char file[32];
    memset(file, 0, sizeof(file));

    strcpy(file, "../");
    strcat(file, REGISTER_MACHINE_FILE);

    if(access(file, F_OK) < 0) {
        fprintf(stderr, "Sorry, no service found. Please register first.\n");
        char logmsg[128]; snprintf(logmsg, sizeof(logmsg), "readfile: File not found: %s\n", file);
        logging(LOGFILE, logmsg);
        return -1;
    }

    if ((fp = fopen(file,"r")) < 0){
        fprintf(stderr, "Sorry, can not get service information.\n");
        char logmsg[128]; snprintf(logmsg, sizeof(logmsg), "readfile: Failed to open file: %s\n", file);
        logging(LOGFILE, logmsg);
        return -1;
    }

    fprintf(stderr, "Server_IP:Port\n");
    while ((read = getline(&line, &len, fp)) != -1) {
        fprintf(stderr, "%s", line);
    }

    fclose(fp);

    return 0;
}

/************
* 3. manager *
************/

int helpServer(){
    fprintf(stderr, "\nUsage: <subcommand> [options]\n");
    fprintf(stderr, "A user interface for the shadow system.\n\n");
    fprintf(stderr, "Subcommands:\n");
    fprintf(stderr, "  help      show this message\n");
    fprintf(stderr, "  register  <program-name> <version-number>\n");
    fprintf(stderr, "            register services into register machine table\n");
    fprintf(stderr, "                <program-name>   the program name provided by this manager\n");
    fprintf(stderr, "                <version-number> version number for the program\n");
    fprintf(stderr, "  quit      stop shadow socket and quit\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "For example:\n");
    fprintf(stderr, "1. To register Map Reduce Library version 1:\n");
    fprintf(stderr, "# register MapReduceLibrary 1\n\n");
    return 0;
}

/************
* 4. client *
************/

int helpClient(){
    fprintf(stderr, "\nUsage: <subcommand> [options]\n");
    fprintf(stderr, "A user interface for the shadow system.\n\n");
    fprintf(stderr, "Subcommands:\n");
    fprintf(stderr, "  help      show this message\n");
    fprintf(stderr, "  request   <program-name> <version-number> <procedure>\n");
    fprintf(stderr, "		request manager info for certain procedure provided by certain program of certain version\n");
    fprintf(stderr, "  execute   <program-name> <version-number> <procedure> <input_file> <out_putfile>\n");
    fprintf(stderr, "            request and call the remote procedure call\n");
    fprintf(stderr, "  quit      stop shadow socket and quit\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "For example:\n");
    fprintf(stderr, "1. To request Multipy in Scientific Library version 2:\n");
    fprintf(stderr, "# request ScientificLibrary 2 Multiply\n");
    fprintf(stderr, "2. To execute Index in Map Reduce Library version 1:\n");
    fprintf(stderr, "# execute MapReduceLibrary 1 Index ../input ../output\n\n");

    return 0;
}
