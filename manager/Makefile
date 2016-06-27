# Makefile for manager

CC = gcc
OS = $(shell uname -s)
WARN = -w
#WARN = -Wall

ifeq (${OS}, SunOS)
	THREADFLAG=-pthreads -lnsl -lsocket -lresolv
else
	THREADFLAG=-pthread
endif

all: manager

manager:
	${CC} ${WARN} -g ${THREADFLAG} -o manager manager.c socket_manager.c socket_register.c ../lib/libsocket.c ../lib/getaddrinfo.c ../lib/liblog.c ../lib/libfile.c ../lib/libterminal.c ../packet/register.c ../packet/checksum.c ../packet/register_ack.c

clean:
ifneq ($(wildcard ../.1*),) 
	rm ../.1* -rf; 
endif
ifneq ($(wildcard manager),)
	rm manager;
endif
ifneq ($(wildcard SRPC.log),)
	rm SRPC.log;
endif
