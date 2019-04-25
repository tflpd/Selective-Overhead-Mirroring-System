#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFERSIZE 1024

int main(int argc, char const *argv[])
{
	if (argc != 7)
	{
		printf("Wrong number of arguments.\n");
		printf("Correct use is:\n");
		printf("./MirrorInitiator -n <MirrorServerAddress> -p <MirrorServerPort> -s <ContentServerAddress1:ContentServerPort1:dirorfile1:delay1\n");
		return 1;
	}

	int i, port, sock, serversPos;
	struct hostent *rem;
	struct in_addr myaddress;
	struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*)&server;
    char *delim, *contentServers;

	for (i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i],"-p"))
		{
			port = atoi(argv[i+1]);
		}else if (!strcmp(argv[i],"-n"))
		{
			if(inet_aton(argv[i+1], &myaddress) == 0){
				if ((rem = gethostbyname(argv[i+1])) == NULL) {	
				   printf("Could not get host.\n");return 1;
			    }
			}else if(inet_aton(argv[i+1], &myaddress) != 0){
				if ((rem = gethostbyaddr((const char*)&myaddress, sizeof(myaddress), AF_INET)) == NULL) {	
				   printf("Could not get host.\n");return 1;
			    }
			}
		}else if (!strcmp(argv[i],"-s"))
		{
			contentServers = malloc(sizeof(char)*(strlen(argv[i+1]) + 1));
			strcpy(contentServers,argv[i+1]);
		}
	}
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    	perror("socket");

    server.sin_family = AF_INET;
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(port);

    if (connect(sock, serverptr, sizeof(server)) < 0)
	   perror("connect");

	printf("Connecting to %s port %d\n", argv[1], port);

	delim = strtok(contentServers,",");
	if ((strlen(delim)*sizeof(char)) >= BUFFERSIZE)
	{
		printf("This content server information has bigger size than available:\n");
		printf("%s\n", delim);
	}else{
		if (write(sock,delim,BUFFERSIZE) < 0)
		{
			perror("write");
		}
	}
	while((delim = strtok(NULL, ",")) != NULL){
		if ((strlen(delim)*sizeof(char)) >= BUFFERSIZE)
		{
			printf("This content server information has bigger size than available:\n");
			printf("%s\n", delim);
		}else{
			if (write(sock,delim,BUFFERSIZE) < 0)
			{
				perror("write");
			}
		}
	}

	close(sock);

	//printf("To name einai: %s\n", rem->h_name);
	return 0;
}