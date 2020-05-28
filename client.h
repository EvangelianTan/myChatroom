#ifndef CLIENT_H
#define CLIENT_H

#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<errno.h>
#include<stdlib.h>
#include<stdio.h>
#include<string>
#include<stdio.h>
#include<cstring>
#include<iostream>
#include<fcntl.h>
#include<pthread.h>


#include "locker.h"

#define MAXLINE 1000

const int MAXCONNECTION=5;
const int MAXRECEIVE=500;
//void* doit(void*);

class client
{  
private:
	int m_port;
	char* host="192.168.80.138";
	char message[MAXLINE];
	char buf[MAXLINE];
	struct sockaddr_in m_address;
	pthread_t tid;
public:
	client();
	~client(  );
	void Connect();
	void perror_exit(const char* s)
	{  
		perror(s);
		exit(1);
	}
};


#endif
