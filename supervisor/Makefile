# Makefile for supervisor

CC = gcc
OS = $(shell uname -s)
WARN = -w # -Wall


ifeq (${OS}, SunOS)
	THREADFLAG=-pthreads -lnsl -lsocket -lresolv
else
	THREADFLAG=-pthread
endif

all:supervisor

supervisor:
	${CC} ${WARN} -g ${THREADFLAG} -o supervisor supervisor.c socket_supervisor.c socket_launch.c socket_dump.c ../lib/libsocket.c ../lib/getaddrinfo.c ../lib/liblog.c ../lib/libfile.c ../lib/libterminal.c ../packet/checksum.c ../packet/launch.c ../packet/dump.c ../packet/register_machine.c ../packet/register_ack.c 

clean:
ifneq ($(wildcard supervisor),)
	rm supervisor;
endif
ifneq ($(wildcard shadow.log),)
	rm shadow.log;
endif
ifneq ($(wildcard ../.register_machine),)
	rm ../.register_machine;
endif
ifneq ($(wildcard ../.1*),) 
	rm ../.1*; 
endif
ifneq ($(wildcard ../.supervisor*),) 
	rm ../.supervisor*; 
endif
