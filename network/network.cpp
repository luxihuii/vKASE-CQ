#include "network.h"

void Server_N::init(const int portno) {
  this->sockfd = server_listen(portno);
}
Server_N::~Server_N(){
  if (sockfd != -1) {
    shutdown(sockfd, 2);
  }
  if (sockfd_new != -1) {
    shutdown(sockfd_new, 2);
  } 
}

int Server_N::server_listen(const int portno) {
  int sockfd;
  struct sockaddr_in serv_addr;

  /* First call to socket() function */

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("ERROR opening socket");
    exit(-1);
  }

  /* Initialize socket structure */

  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  /* Now bind the host address using bind() call.*/

	int flag = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) < 0)
	{
		printf("socket setsockopt error\n");
		exit(1);
	}

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("ERROR on binding");
    exit(-1);
  }

  /* Now start listening for the clients, here process will
     go in sleep mode and will wait for the incoming connection */

  listen(sockfd, 1);

  return sockfd;
}

void Server_N::server_accept() {
  socklen_t clilen;
  struct sockaddr_in cli_addr;
  clilen = sizeof(cli_addr);
  sockfd_new = accept(this->sockfd, (struct sockaddr *)&cli_addr, &clilen);
  if (sockfd_new < 0) {
    perror("ERROR on accept");
    exit(-1);
  }
}

void Client_N::init(const char* host, const int portno) {
  client_connect(&sockfd, host, portno);
}

Client_N::~Client_N(){
  if (sockfd != -1) {
    shutdown(sockfd, 2);
  }
}

void Client_N::client_connect(int *sock, const char *host, const int portno) {
  int sockfd;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  /* Create a socket point */

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) {
    perror("ERROR opening socket");
    exit(-1);
  }

  /* deal with host */

  server = gethostbyname(host);

  if (server == NULL) {
    fprintf(stderr, "ERROR, no such host\n");
    exit(0);
  }

  memset(&serv_addr, 0, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;

  memcpy(&(serv_addr.sin_addr.s_addr), server->h_addr, server->h_length);

  serv_addr.sin_port = htons(portno);

  /* Now connect to the server */

  while (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    ;

  *sock = sockfd;
}

void Server_N::send(void *buffer, const unsigned len) {
  unsigned delivered = 0;
  int n;
  char *ptr = (char *)buffer;

  /* Send message to the server */

  while (delivered < len) {
    n = write(sockfd_new, ptr, len - delivered);

    if (n < 0) {
      perror("ERROR writing to socket");
      exit(-1);
    }

    else {
      delivered += n;
      ptr += n;
    }
  }
}

void Server_N::receive(void *buffer, const unsigned len) {
  unsigned delivered = 0;
  int n;
  char *ptr = (char *)buffer;

  /* Send message to the server */

  while (delivered < len) {
    n = read(sockfd_new, ptr, len - delivered);

    if (n < 0) {
      perror("ERROR reading from socket");
      exit(-1);
    }

    else {
      delivered += n;
      ptr += n;
    }
  }
}

void Client_N::send(void *buffer, const unsigned len) {
  unsigned delivered = 0;
  int n;
  char *ptr = (char *)buffer;

  /* Send message to the server */

  while (delivered < len) {
    n = write(sockfd, ptr, len - delivered);

    if (n < 0) {
      perror("ERROR writing to socket");
      exit(-1);
    }

    else {
      delivered += n;
      ptr += n;
    }
  }
}

void Client_N::receive(void *buffer, const unsigned len) {
  unsigned delivered = 0;
  int n;
  char *ptr = (char *)buffer;

  /* Send message to the server */

  while (delivered < len) {
    n = read(sockfd, ptr, len - delivered);

    if (n < 0) {
      perror("ERROR reading from socket");
      exit(-1);
    }

    else {
      delivered += n;
      ptr += n;
    }
  }
}

// network_base::network_base(int n) {
//   server.reserve(n);
//   for (int i = 0; i < n; ++i) {
//     serverServer ser(DEFPORT + i);

//   }
// }