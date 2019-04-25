#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFERSIZE 1024

#define POOL_SIZE 6

typedef struct
{
	char *data;
	char *address;
} request;

typedef struct {
	request requests[POOL_SIZE];
	int start;
	int end;
	int count;
	int flag;
} pool_t;

pthread_mutex_t mtxContent;
pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;
pool_t pool;

int read_data (int fd, char **buffer){/* Read formated data */
	char temp[16], *myb; int i = 0, length = 0;
	if ( read ( fd, temp, 16 ) < 0 )/* Get length of string */
		exit (-3);
	//printf("TO LEN %s\n", temp);
	length = atoi(temp);
	*buffer = malloc(sizeof(char)*(length + 1));
	myb = *buffer;
	while ( i < length )	/* Read $length chars */
		if ( i < ( i+= read (fd, &myb[i], length - i) ) )
			exit (-3);
	return i;	/* Return size of string */
}

int write_all(int fd, void *buff, size_t size) {
    int sent, n;
    for(sent = 0; sent < size; sent+=n) {
        if ((n = write(fd, buff+sent, size-sent)) == -1) 
            return -1; /* error */
    }
    return sent;
}

void initialize(pool_t * pool) {
	int i;
	for (i = 0; i < POOL_SIZE; ++i)
	{
		pool->requests[i].data = NULL;
		pool->requests[i].address = NULL;
	}
	pool->start = 0;
	pool->end = -1;
	pool->count = 0;
	pool->flag = 0;
}

void place(pool_t * pool, char *data, char *address) {
	pthread_mutex_lock(&mtxContent);
	while (pool->count >= POOL_SIZE) {
		//printf(">> Found Buffer Full \n");
		pthread_cond_wait(&cond_nonfull, &mtxContent);
		}
	pool->end = (pool->end + 1) % POOL_SIZE;
	pool->requests[pool->end].data = malloc((strlen(data) + 1)*sizeof(char));
	strcpy(pool->requests[pool->end].data,data);
	pool->requests[pool->end].address = malloc((strlen(address) + 1)*sizeof(char));
	strcpy(pool->requests[pool->end].address,address);
	pool->count++;
	pthread_mutex_unlock(&mtxContent);
}

void obtain(pool_t * pool,char **data,char **address) {
	*data = NULL;
	*address = NULL;
	//printf("MPIKA\n");
	pthread_mutex_lock(&mtxContent);
	//printf("TOPIRA\n");
	while (pool->count <= 0) {
		//printf(">> Found Buffer Empty \n");
		if (pool->flag)
		{
			break;
		}
		pthread_cond_wait(&cond_nonempty, &mtxContent);
	}
	//printf("KSESKALWSA\n");
	if (pool->count > 0)
	{
		*data = malloc((strlen(pool->requests[pool->start].data) + 1)*sizeof(char));
		strcpy(*data,pool->requests[pool->start].data);
		free(pool->requests[pool->start].data);
		*address = malloc((strlen(pool->requests[pool->start].address) + 1)*sizeof(char));
		strcpy(*address,pool->requests[pool->start].address);
		free(pool->requests[pool->start].address);
		pool->start = (pool->start + 1) % POOL_SIZE;
		pool->count--;
	}
	pthread_mutex_unlock(&mtxContent);
	return;
}


void * workers(void * ptr)
{
	while (pool.count > 0 || pool.flag == 0) {
		//printf("EMPENE me %d %d\n", pool.count, pool.flag);
		char *data, *address, *contentName;
		obtain(&pool, &data, &address);
		if (data != NULL && address != NULL)
		{
			pthread_cond_broadcast(&cond_nonfull);

			char *constData, *tmpSave, *delim, *dir, *fetchBuf, *adr, *portC, *constFile;
			int fd;



    		char *savePtr;
		    int sock, port;
		    struct hostent *rem;
		    struct in_addr myaddress;
		    struct sockaddr_in server;
		    struct sockaddr *serverptr = (struct sockaddr*)&server;

		    //strcpy(tmpBuf,buf);
			delim = strtok_r(address, ":",&savePtr);
			adr = malloc(sizeof(char)*(strlen(delim) + 1));
			strcpy(adr, delim);
			/*address = malloc(sizeof(char)*(strlen(delim) + 1));
			strcpy(address,delim);*/
			if(inet_aton(delim, &myaddress) == 0){
				if ((rem = gethostbyname(delim)) == NULL) {	
				   printf("Could not get host.\n");pthread_exit(NULL);
			    }
			}else if(inet_aton(delim, &myaddress) != 0){
				if ((rem = gethostbyaddr((const char*)&myaddress, sizeof(myaddress), AF_INET)) == NULL) {	
				   printf("Could not get host.\n");pthread_exit(NULL);
			    }
			}

			delim = strtok_r(NULL, ":",&savePtr);
			portC = malloc(sizeof(char)*(strlen(delim) + 1));
			strcpy(portC, delim);
			port = atoi(delim);
			

			contentName = malloc(sizeof(char)*(strlen(adr) + (strlen(portC)) + 4 + strlen((char*)ptr) ));
			sprintf(contentName,"%s/%s_%s/",(char*)ptr,adr,portC);
			free(adr);
			free(portC);

			//printf("THA FTIAKSW %s\n", contentName);
			mkdir(contentName, 0777);

			char *constData2;
			constData = malloc(sizeof(char)*(strlen(data) + 1));
			strcpy(constData, data);
			constData2 = malloc(sizeof(char)*(strlen(data) + 1));
			strcpy(constData2, data);
    		delim = strtok_r(data, "/",&tmpSave);
    		delim = strtok_r(NULL, "/",&tmpSave);
    		dir = malloc(sizeof(char)*(strlen(delim) + strlen(contentName) + 1));
    		strcpy(dir,contentName);
    		strcat(dir, delim);
    		while((delim = strtok_r(NULL, "/",&tmpSave)) != NULL){
    			mkdir(dir, 0777);
    			dir = realloc(dir, sizeof(char)*(strlen(dir) + strlen(delim) + 2));
    			strcat(dir, "/");
    			strcat(dir, delim);
    		}

    		delim = strtok_r(constData, "/",&tmpSave);
    		delim = strtok_r(NULL, "\n",&tmpSave);
    		constFile = malloc(sizeof(char)*(strlen(contentName) + strlen(delim) + 1));
    		sprintf(constFile,"%s%s",contentName,delim);
    		free(contentName);

    		if ((fd = open(constFile, O_WRONLY | O_CREAT | O_EXCL, 0777)) < 0)
    		{	
    			perror("open");
    			printf("%s\n", constData);
    		}

			if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
				perror("socket");
			server.sin_family = AF_INET;
		    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
		    server.sin_port = htons(port);

		    if (connect(sock, serverptr, sizeof(server)) < 0)
		   		perror("connect");

		   	fetchBuf = malloc(sizeof(char)*(strlen(constData2) + strlen("FETCH") + 2));
			sprintf(fetchBuf,"FETCH.%s",constData2);
		   	if ( write_all(sock, fetchBuf, strlen(fetchBuf)) == -1)
		        perror("write");
		    if ( write_all(sock, "\n", 1) == -1 )
		        perror("write");

		    char *bytesBuf;
		    int size, bytesRead;

		    bytesBuf = malloc (sizeof(char)*(257));

		    if(read(sock, bytesBuf, 257) < 0)
		    	perror("read");
		   	sscanf(bytesBuf,"%d",&size);
		   	free(bytesBuf);
		   	
		   	int counter = 0;
		   	while(counter < size){
		   		bytesBuf = malloc (sizeof(char)*(257));
		   		if ((bytesRead = read(sock,bytesBuf,257)) < 0)
		   			perror("read");
		   		//printf("PIRA:%s :%d\n", bytesBuf, bytesRead);
		   		write(fd,bytesBuf,bytesRead);
		   		counter+=257;
		   		free(bytesBuf);
		   	}

    		close(fd);
    		close(sock);
    		free(dir);
			free(data);
			free(constData);
			free(fetchBuf);
			free(constFile);
			free(constData2);
		}
	}
	pthread_exit(0);
}

void *managers(void *buf) {
	char *delim, tmpBuf[BUFFERSIZE], *savePtr, *filePath, listBuf[32], *address, *portChar, *finalAddress;
    int sock, port;
    struct hostent *rem;
    struct in_addr myaddress;
    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*)&server;

    //printf("Thread %ld: Thread Created and will execute %s\n", pthread_self(), (char *)buf);

    strcpy(tmpBuf,buf);
	delim = strtok_r(tmpBuf, ":",&savePtr);
	address = malloc(sizeof(char)*(strlen(delim) + 1));
	strcpy(address,delim);
	if(inet_aton(delim, &myaddress) == 0){
		if ((rem = gethostbyname(delim)) == NULL) {	
		   printf("Could not get host.\n");pthread_exit(NULL);
	    }
	}else if(inet_aton(delim, &myaddress) != 0){
		if ((rem = gethostbyaddr((const char*)&myaddress, sizeof(myaddress), AF_INET)) == NULL) {	
		   printf("Could not get host.\n");pthread_exit(NULL);
	    }
	}

	delim = strtok_r(NULL, ":",&savePtr);
	portChar = malloc(sizeof(char)*(strlen(delim) + 1));
	strcpy(portChar,delim);

	finalAddress = malloc(sizeof(char)*(strlen(address) + strlen(portChar) + 2));
	sprintf(finalAddress,"%s:%s",address,portChar);
	free(address);
	free(portChar);

	port = atoi(delim);
	delim = strtok_r(NULL, ":",&savePtr);
	filePath = malloc(sizeof(char)*(strlen(delim)+1));
	strcpy(filePath,delim);
	delim = strtok_r(NULL, ":",&savePtr);
	sprintf(listBuf,"LIST.%s.",delim);


	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		perror("socket");
	server.sin_family = AF_INET;
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(port);

    if (connect(sock, serverptr, sizeof(server)) < 0){
    	printf("Connection with Content Server was not made.\n");
    	printf("Content Server was not available.\n");
   		perror("connect");
    }

	if ( write_all(sock, listBuf, strlen(listBuf)) == -1)
        perror("write");
    if ( write_all(sock, "\n", 1) == -1 )
        perror("write");


    while(1){
    	char *myBuf;
    	read_data(sock,&myBuf);
    	if (!strcmp(myBuf,"#teliwsa"))
    	{    		
    		free(myBuf);
    		break;
    	}
    	//printf("Thread %ld:Comparing %s to %s\n", pthread_self(), myBuf, delim);
    	if (strstr(myBuf,filePath) != NULL)
    	{
    		place(&pool, myBuf, finalAddress);
    		//printf("ThreadProd %ld: %s %s\n", pthread_self(), myBuf, finalAddress);
			pthread_cond_signal(&cond_nonempty);
    	}
    	free(myBuf);
    }
	close(sock);
	free(buf);
	free(filePath);
	free(finalAddress);
	pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
	if (argc != 7)
	{
		printf("Wrong number of arguments.\n");
		printf("Correct use is:\n");
		printf("/MirrorServer -p <port> -m <dirname> -w <threadnum>\n");
		return 1;
	}

	int i, port, threadNum, sock, newsock, clientlen;
	char *destPath;
	struct sockaddr_in server, client;
	struct sockaddr *serverptr=(struct sockaddr *)&server;
    struct sockaddr *clientptr=(struct sockaddr *)&client;

    initialize(&pool);
	pthread_mutex_init(&mtxContent, 0);
	pthread_cond_init(&cond_nonempty, 0);
	pthread_cond_init(&cond_nonfull, 0);

	for (i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i],"-p"))
		{
			port = atoi(argv[i+1]);
		}else if (!strcmp(argv[i],"-w"))
		{
			threadNum = atoi(argv[i+1]);
		}else if (!strcmp(argv[i],"-m"))
		{
			destPath = malloc(sizeof(char)*(strlen(argv[i+1]) + 1));
			strcpy(destPath,argv[i+1]);
			mkdir(destPath,0777);
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

    if ((newsock = accept(sock, NULL, NULL)) < 0) perror("accept");

    printf("Accepted connection\n");


	char buf[BUFFERSIZE];
    pthread_t *thrManagers = NULL;
    pthread_t *thrWorkers = NULL;
    int err, mirrorManagers = 0, limit = 16;

    thrManagers = malloc(sizeof(pthread_t)*limit);
    thrWorkers = malloc(sizeof(pthread_t)*threadNum);

    for (i = 0; i < threadNum; ++i)
    {
		if (err = pthread_create(&thrWorkers[i], NULL, workers, destPath)){
			perror("pthread_create"); return(1);
      	}
    }

    char *tmpBuf;
    
    while(read(newsock, buf, BUFFERSIZE) > 0) {
    	tmpBuf = malloc(sizeof(char)*BUFFERSIZE);
    	strcpy(tmpBuf,buf);
    	if (err = pthread_create(&thrManagers[mirrorManagers], NULL, managers, tmpBuf)) { /* New thread */
      		perror("pthread_create"); return(1); }
      	mirrorManagers++;
      	if (mirrorManagers == limit)
      	{
      		limit*=2;
      		thrManagers = realloc(thrManagers, sizeof(pthread_t)*limit);
      	}
    }
    for (i = 0; i < mirrorManagers; ++i)
    {
    	if (err = pthread_join(thrManagers[i], NULL)) { /* Wait for thread */
       		perror("pthread_join"); return(1); } /* termination */	
    }
    pthread_mutex_lock(&mtxContent);
    pool.flag = 1;
    pthread_mutex_unlock(&mtxContent);
    pthread_cond_broadcast(&cond_nonempty);

    for (i = 0; i < threadNum; ++i)
    {
    	if (err = pthread_join(thrWorkers[i], NULL)) { /* Wait for thread */
       		perror("pthread_join"); return(1); } /* termination */	
    }
    pthread_cond_destroy(&cond_nonempty);
	pthread_cond_destroy(&cond_nonfull);
	pthread_mutex_destroy(&mtxContent);
	close(newsock);
	close(sock);
	free(destPath);

	return 0;
}