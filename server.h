#ifndef SERVER_H
#define SERVER_H

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<errno.h>
#include<fcntl.h>
#include<stdlib.h>
#include<cassert>
#include<sys/epoll.h>
#include<sys/socket.h>
#include<string>
#include<sys/types.h>
#include<unistd.h>
#include<map>
#include<string.h>
#include<iostream>

#include "threadpool.h"
#include "locker.h"

const int MAX_FD=1000;
const int MAX_EVENT_NUMBER=10000;
const int TIMEOUT=3;
#define MAXLINE 1000

class server
{  
public:
	server();
	~server();
	void perr_exit(const char* s)
	{  
		perror(s);
		exit(1);
	};
	void server_run();
	//bool addClient(int sockfd,struct sockaddr_in& addr);
	//void deleteClient(int sockfd);
	//void processMessage(int sockfd,char[]);
	//void sendToAllUsers(char message[]);

private:
	int m_port;
	int m_epollfd;
	int m_thread_num;
	int m_listenfd;
	//threadpool m_threadpool(MAX_FD,MAX_FD);
	char str[INET_ADDRSTRLEN];
	struct sockaddr_in servaddr,cliaddr;

	epoll_event events[MAX_EVENT_NUMBER],tep;
	//std::map<int,struct sockaddr_in*>clientSockets;
};

#endif
