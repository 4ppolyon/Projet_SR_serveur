.PHONY: all, clean

# Disable implicit rules
.SUFFIXES:

# Keep intermediate files
#.PRECIOUS: %.o

# Variable pour compilation
CC=gcc
CFLAGS= -g -Wall
CPPFLAGS = -I$(HDRPATH)

# Note: -lnsl does not seem to work on Mac OS but will
# probably be necessary on Solaris for linking network-related functions 
#LIBS += -lsocket -lnsl -lrt
LDFLAGS+=-lpthread

# Variable pour chemin
BINPATH=bin
SRCPATH=source
HDRPATH=header
OBJPATH=objet

# Variable pour les cibles et la récupération des fichiers
SRCS=$(wildcard $(SRCPATH)/*.c)
# OBJS=$(SRCS:$(SRCPATH)/%.c=$(OBJPATH)/%.o)
OBJS=$(OBJPATH)/ftp.o $(OBJPATH)/csapp.o $(OBJPATH)/f_client.o
INCLUDE = $(wildcard $(HDRPATH)/*.c)
EXEC_cli=$(BINPATH)/client 
EXEC_serv_master=$(BINPATH)/serveur_maitre
EXEC_serv_slave=$(BINPATH)/serveur_esclave

all:  $(EXEC_cli) $(EXEC_serv_master) $(EXEC_serv_slave)

$(OBJPATH)/%.o: $(SRCPATH)/%.c
		$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

$(EXEC_cli): $(OBJS) $(OBJPATH)/client.o
		$(CC) $(CFLAGS) $(OBJS) $(OBJPATH)/client.o -o $(EXEC_cli) $(LDFLAGS)

$(EXEC_serv_master): $(OBJS) $(OBJPATH)/serveur_maitre.o
		$(CC) $(CFLAGS) $(OBJS) $(OBJPATH)/serveur_maitre.o -o $(EXEC_serv_master) $(LDFLAGS)

$(EXEC_serv_slave): $(OBJS) $(OBJPATH)/serveur_esclave.o
		$(CC) $(CFLAGS) $(OBJS) $(OBJPATH)/serveur_esclave.o -o $(EXEC_serv_slave) $(LDFLAGS)


clean:
	rm -f  $(EXEC_cli)  $(EXEC_serv_master) $(EXEC_serv_slave) $(OBJPATH)/*.o

