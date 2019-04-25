MINITIATOR = MirrorInitiator.o
MSERVER = MirrorServer.o
CSERVER = ContentServer.o
OUTMINITIATOR = MirrorInitiator
OUTMSERVER = MirrorServer
OUTCSERVER = ContentServer
HEADER = 
CC = gcc
FLAGS  = -g -c

all: $(OUTMINITIATOR) $(OUTMSERVER) $(OUTCSERVER)

$(OUTMINITIATOR): $(MINITIATOR)
	$(CC) -g $(MINITIATOR) -o $@

$(OUTMSERVER): $(MSERVER)
	$(CC) -g $(MSERVER) -o $@ -lpthread

$(OUTCSERVER): $(CSERVER)
	$(CC) -g $(CSERVER) -o $@

MirrorInitiator.o: MirrorInitiator.c
	$(CC) $(FLAGS) MirrorInitiator.c

MirrorServer.o: MirrorServer.c
	$(CC) $(FLAGS) MirrorServer.c

ContentServer.o: ContentServer.c
	$(CC) $(FLAGS) ContentServer.c

clean: 
	rm -f $(OUTMINITIATOR) $(OUTMSERVER) $(OUTCSERVER) $(MINITIATOR) $(MSERVER) $(CSERVER)
