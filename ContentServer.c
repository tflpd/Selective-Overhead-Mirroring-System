#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFFERSIZE 1024

int write_data ( int fd, char* message ){/* Write formated data */
	char temp[16]; int length = 0;
	length = strlen(message) + 1;	/* Find length of string */
	//temp = length;
	sprintf(temp,"%d",length);
	//printf("to len %s\n", temp);
	if ( write (fd, temp, 16) < 0 ){
		perror("write1");
		exit (-2);
	}	/* Send length first */
	if ( write (fd, message, length) < 0 ){
		perror("write2");
		exit (-2);
	}	/* Send string */
	return length;				/* Return size of string */
}

int write_all(int fd, void *buff, size_t size) {
    int sent, n;
    for(sent = 0; sent < size; sent+=n) {
        if ((n = write(fd, buff+sent, size-sent)) == -1) 
            return -1; /* error */
    }
    return sent;
}

int main(int argc, char const *argv[])
{
	if (argc != 5)
	{
		printf("Wrong number of arguments.\n");
		printf("Correct use is:\n");
		printf("./ContentServer -p <port> -d <dirorfilename>\n");
		return 1;
	}

	int i, port, threadNum, sock, newsock, clientlen, delay;
	char *availablePath, function[64], *findFunct;
	struct sockaddr_in server, client;
	struct sockaddr *serverptr=(struct sockaddr *)&server;
    struct sockaddr *clientptr=(struct sockaddr *)&client;
    FILE *sock_fp, *pipe_fp;

	for (i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i],"-p"))
		{
			port = atoi(argv[i+1]);
		}else if (!strcmp(argv[i],"-d"))
		{
			availablePath = malloc(sizeof(char)*(strlen(argv[i+1]) + 1));
			strcpy(availablePath,argv[i+1]);
			findFunct = malloc(sizeof(char)*(strlen(availablePath) + strlen("find -type f") + 2));
			sprintf(findFunct, "find %s -type f", availablePath);
		}
	}

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror("socket");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);

    if (bind(sock, serverptr, sizeof(server)) < 0)
        perror("bind");

    if (listen(sock, 5) < 0) perror("listen");
    printf("Listening for connections to port %d\n", port);

    while(1){
    	if ((newsock = accept(sock, NULL, NULL)) < 0) perror("accept");

	    if ((sock_fp = fdopen(newsock,"r+")) == NULL)
	    	perror("fdopen");

	    //printf("Accepted connection\n");

	    if (fgets(function, BUFFERSIZE, sock_fp) == NULL){
	    	perror("reading function");
	    }else{
	    	char *delim = NULL;
	    	delim = strtok(function,".");
	    	if (!strcmp(delim,"LIST"))
			{
				delay = atoi(strtok(NULL,"\n"));
				printf("Delay is %d\n", delay);
				char buf[BUFFERSIZE], lbuf[64], newBuf[BUFFERSIZE];
				int len = 0;
				if ((pipe_fp = popen("find -type f", "r")) == NULL )
					perror("popen");
				while( fgets(buf, BUFFERSIZE, pipe_fp) != NULL ){
					write_data(newsock,strtok(buf,"\n"));
				}
				write_data(newsock,"#teliwsa");
				pclose(pipe_fp);
			}else if (!strcmp(delim,"FETCH"))
			{	
				//printf("1\n");
				sleep(delay);
				char *filePath, *myDelim, *bytesBuf = NULL;
				int fd, bytesRead = 0;

				bytesBuf = malloc (sizeof(char)*(257));

				myDelim = strtok(NULL,"\n");
				//printf("MYD %s\n", myDelim);
				filePath = malloc(sizeof(char)*(strlen(myDelim) + 1));
				strcpy(filePath, myDelim);

				if ((fd = open(filePath, O_RDONLY)) < 0)
	    		{
	    			perror("open");
	    		}

	    		struct stat bufferS;
	    		fstat(fd, &bufferS);

	    		int size = bufferS.st_size;
	    		char fileSize[257];
	    		sprintf(fileSize,"%d", size);


	    		if(write(newsock,fileSize,257) < 0)
	    			perror("write");

	    		while((bytesRead = read(fd, bytesBuf, 257)) > 0){
	    			int tst;
	    			while((tst = write(newsock, bytesBuf, bytesRead)) < 0);
	    			//printf("EDWSA:%s :%d\n", bytesBuf, bytesRead);
	    		}
	    		close(fd);

				free(filePath);
				free(bytesBuf);
			}
			fclose(sock_fp);
			close(newsock);	
	    }
    }
    
	close(sock);
	free(availablePath);
	free(findFunct);
	return 0;
}