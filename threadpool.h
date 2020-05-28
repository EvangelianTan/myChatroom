#ifndef THREADPOOL_H
#define THREADPOOL_H

#include<list>
#include<cstdio>
#include<pthread.h>
#include<exception>
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



#include "locker.h"
#define MAXLINE 1000

using namespace std;

void perr_exit(const char* s)
{  
	perror(s);
	exit(1);
}


class threadpool
{  
public:
	threadpool(int thread_number,int max_request,int epollfd);
	~threadpool(  );
	bool append(int sockfd);
	bool addClient(int sockfd,struct sockaddr_in& addr);
    void deleteClient(  int sockfd);
	void processMessage(  int sockfd,char[  ]);
	void sendToAllUsers(  char message[  ]);
	void run();

private:
	static void* worker(void *arg);
	int m_thread_number;
	int m_max_requests;
	int m_epollfd;
	int m_sockfd;
	pthread_t* m_threads;
	locker m_queuelocker;
	locker m_clients_map;
	std::list<int> m_workqueue;
	sem m_queuestat;
	std::map<int,struct sockaddr_in*>clientSockets;
};

threadpool::threadpool(int thread_number,int max_request,int epollfd):m_thread_number(thread_number),m_max_requests(max_request),m_epollfd(epollfd)
{  
	if(thread_number<=0||max_request<=0 )
		throw std::exception();
	m_threads=new pthread_t[m_thread_number];
	if( !m_threads)
	{  
		throw std::exception();
	}
	for(int i=0;i<thread_number;i++)
	{  
		if(pthread_create(m_threads+i,NULL,worker,this)!=0)
		{  
			delete[] m_threads;
			throw std::exception();
		}
	}
}


threadpool::~threadpool(  )
{  
	delete[] m_threads;
	std::map<int,struct sockaddr_in*>::const_iterator i;
	for(  i=clientSockets.begin(  );i!=clientSockets.end(  );i++)
	{    
		delete i->second;
		close(i->first);
	}   
}

bool threadpool::append(int sockfd)
{  
	m_queuelocker.lock();
	//m_sockfd=sockfd;
	if(m_workqueue.size()>=m_max_requests)
	{  
		m_queuelocker.unlock();
		return false;
	}
	m_workqueue.push_back(sockfd);
	m_queuelocker.unlock();
	m_queuestat.post();
	return true;
}

void* threadpool::worker(void* arg)
{  
	threadpool* pool=(threadpool*)arg;
	pool->run();
	return pool;
}

void threadpool::run()
{  
	while(true)
	{  
		m_queuestat.wait();
		m_queuelocker.lock();
		if(m_workqueue.empty())
		{  
			m_queuelocker.unlock();
			continue;
		}
		m_sockfd=m_workqueue.front();
		m_workqueue.pop_front();
		m_queuelocker.unlock();
		char buf[MAXLINE],str[INET_ADDRSTRLEN];
		const char* r="exit";
		while(true)
		{  
			memset(buf,'\0',sizeof(  buf));
			int n=read(m_sockfd,buf,MAXLINE);
			if(n==-1)
			{    
				perr_exit("read_error");
				deleteClient(m_sockfd);
				//int res=epoll_ctl(m_epollfd,EPOLL_CTL_DEL,m_sockfd,NULL);	
				//if(  res==-1)
				//{    
				//	perr_exit(  "  epoll_ctl_del"  );
				//}
				printf("client %s at port %d  closed connection\n",inet_ntop(  AF_INET,&clientSockets[m_sockfd]->sin_addr,str,sizeof(str)),ntohs(clientSockets[m_sockfd]->sin_port));
			}
			else if(n>0)
			{    
				processMessage(m_sockfd,buf);
				if(strncasecmp(buf,r,4)==0)
				{  
					cout<<"equal"<<endl;
					deleteClient(m_sockfd);
					close(m_sockfd);
					pthread_exit((void *)1);
				}
			}
		}
	}
}

bool threadpool::addClient(  int sockfd,struct sockaddr_in& addr)
{    
	m_clients_map.lock();
	if(clientSockets.find(  sockfd)==clientSockets.end(  ))
	{  
		clientSockets[sockfd]=&addr;
		m_clients_map.unlock();
		return true;
	}
	else
	{    
		cout<<"  this client has existed"  <<endl;;
		return false;
	}
}

void threadpool::sendToAllUsers(char* mess)
{  
	cout<<mess;
	map<int,struct sockaddr_in*>::const_iterator i;
	int count=0;
	m_clients_map.lock();
	for(i=clientSockets.begin();i!=clientSockets.end();i++)
	{     
		count++;
		if(write((i->first),mess,strlen(mess))<0)
		{    
			deleteClient(i->first);
			cout<<"  send error"  <<endl;
			continue;
		}
		cout<<"send to user "  <<count<<endl;
	}
	cout<<"count is: "<<count<<endl;
	m_clients_map.unlock();
}


void threadpool::deleteClient(int sockfd)
{   
	m_clients_map.lock();
	//delete clientSockets[sockfd];
	clientSockets.erase(sockfd);
	m_clients_map.unlock();
	int res=epoll_ctl(m_epollfd,EPOLL_CTL_DEL,sockfd,NULL);
	if(res<0)
	{    
		perr_exit("epol_ctl_del error");
	}
	close(sockfd);
}


void threadpool::processMessage(int sockfd,char* mess)
{    
	char info_host[INET_ADDRSTRLEN];
	inet_ntop(AF_INET,&clientSockets[sockfd]->sin_addr,info_host,sizeof(  info_host));
	char send_mess[MAXLINE]="received from ";
	strcat(send_mess,info_host);
	strcat(send_mess," at port ");
	char s[MAXLINE];
	sprintf(s,"%d",ntohs(clientSockets[sockfd]->sin_port));
	strcat(send_mess,s);
	strcat(send_mess," : "  );
	strcat(send_mess,mess);
	//cout<<"send-message is:"  <<send_mess<<endl;
	sendToAllUsers(send_mess);
}

#endif
