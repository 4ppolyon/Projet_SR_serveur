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
OBJS=$(OBJPATH)/ftp.o $(OBJPATH)/csapp.o
INCLUDE = $(wildcard $(HDRPATH)/*.c)
EXEC_cli=$(BINPATH)/client 
EXEC_serv=$(BINPATH)/serveur

all:  $(EXEC_cli) $(EXEC_serv)

$(OBJPATH)/%.o: $(SRCPATH)/%.c
		$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

$(EXEC_cli): $(OBJS) $(OBJPATH)/client.o
		$(CC) $(CFLAGS) $(OBJS) $(OBJPATH)/client.o -o $(EXEC_cli) $(LDFLAGS)

$(EXEC_serv): $(OBJS) $(OBJS) $(OBJPATH)/serveur.o
		$(CC) $(CFLAGS) $(OBJS) $(OBJPATH)/serveur.o -o $(EXEC_serv) $(LDFLAGS)

clean:
	rm -f  $(EXEC_cli) $(EXEC_serv) $(OBJPATH)/*.o

