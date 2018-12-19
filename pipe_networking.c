#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
	char store[HANDSHAKE_BUFFER_SIZE];
	mkfifo("wkp", 0600);

	printf("Waiting for DE client...\n");
	int wkpid =  open("wkp", O_RDONLY | O_CREAT, 0600);

	if(wkpid + 1){
		printf("SERVER: well-known-pipe is open!\n");
	}
	else{
		printf("SERVER: error makin well-known-pipe [%s]\n", strerror(errno));
		remove("wkp");
		exit(EXIT_FAILURE);
	}

	int res = read(wkpid, store, HANDSHAKE_BUFFER_SIZE);
	
	if(res == -1){
		printf("Something bad happened with Reading from Pipe");
		remove("wkp");
		close(wkpid);
		exit(EXIT_FAILURE);
	}

	printf("Pipe name has been gotten: %s\n", store);
	
	*to_client = open(store, O_WRONLY);

	if(*to_client + 1){
		printf("SERVER: successfully opened!\n");
	}
	else{
		printf("SERVER: error opening [%s]\n", strerror(errno));
	}

	remove("wkp");
	write(*to_client, ACK, sizeof(ACK));
	printf("SERVER: sending message [%s]\n", ACK);
	
	read(wkpid, store, HANDSHAKE_BUFFER_SIZE);
	
	if(strncmp(store, ACK, HANDSHAKE_BUFFER_SIZE)==0){
		printf("CONFIRMED [%s]\n", store);
	}
	else{
		printf("NOT CONFIRMED [%s] VS [%s]", store, ACK);
		close(wkpid);
		close(*to_client);
		exit(EXIT_FAILURE);
	}
	

  	return wkpid;

}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
	char store[HANDSHAKE_BUFFER_SIZE];
	int pkw;
	mkfifo("pkw", 0600);

	*to_server = open("wkp", O_WRONLY);
	if(*to_server + 1){
		printf("CLIENT: well-known-pipe open SUCCESS!\n");
	}
	else{
		printf("CLIENT: ERROR OPENING well-known-pipe\n");
		exit(0);
	}
	
	printf("CLIENT: sending message [pkw] through!\n");
	write(*to_server, "pkw", sizeof("pkw"));
	
	pkw = open("pkw", O_RDONLY, 0600);
	if(pkw != -1){
		printf("CLIENT: pkw created!\n");
	}
	else{
		printf("CLIENT: error when creating pkw\n");
	}

	read(pkw, store, HANDSHAKE_BUFFER_SIZE);
	printf("CLIENT: received message [%s]\n", store);

	remove("pkw");
	write(*to_server, ACK, sizeof(ACK));
	return pkw;

}
