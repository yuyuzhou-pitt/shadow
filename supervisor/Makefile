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
	${CC} ${WARN} -g ${THREADFLAG} -o supervisor supervisor.c socket_supervisor.c ../lib/libsocket.c ../lib/getaddrinfo.c ../lib/liblog.c ../lib/libfile.c ../lib/libterminal.c ../packet/checksum.c ../packet/register_machine.c ../packet/register_ack.c ../packet/register.c

clean:
ifneq ($(wildcard supervisor),)
	rm supervisor;
endif
ifneq ($(wildcard SRPC.log),)
	rm SRPC.log;
endif
ifneq ($(wildcard ../.supervisor*),) 
	rm ../.supervisor*; 
endif
