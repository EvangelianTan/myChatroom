#include "server.h"


#define OPEN_MAX 1000
using namespace std;
server::server(  )
{  
	m_port=9007;
	m_listenfd=socket(AF_INET,SOCK_STREAM,0);
	if(  m_listenfd<0)
		perr_exit("socket_creat error");
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(m_port);

	bind(m_listenfd,(struct sockaddr*) &servaddr,sizeof(servaddr));
	//threadpool m_threadpool(MAX_FD,MAX_FD,m_epollfd);
	listen(m_listenfd,20);
	m_epollfd=epoll_create(OPEN_MAX);
	if(m_epollfd==-1)
	{  
		perr_exit("epoll_create");
	}
	tep.events=EPOLLIN;
	tep.data.fd=m_listenfd;
	int res=epoll_ctl(m_epollfd,EPOLL_CTL_ADD,m_listenfd,&tep);
	//threadpool m_threadpool(MAX_FD,MAX_FD,m_epollfd);
	if(res==-1)
	{  
		perr_exit("epoll_ctl");
	}
}

void server::server_run()
{  
	cout<<"server is runing"<<endl;
	int nready,i;
	threadpool m_threadpool(MAX_FD,MAX_FD,m_epollfd);

	while(1)
	{  
		nready=epoll_wait(m_epollfd,events,OPEN_MAX,-1);
		if(nready==-1)
		{  
			perr_exit("epoll_wait");
		}
		for(i=0;i<nready;i++)
		{  
			if(!(events[i].events & EPOLLIN))
			{  
				m_threadpool.deleteClient(events[i].data.fd);
				continue;
			}
			if(events[i].data.fd==m_listenfd)
			{  
				int len=sizeof(cliaddr);
				int confd=accept(m_listenfd,(struct sockaddr*)&cliaddr,(socklen_t *)&len);
				if(confd<0)
					perr_exit("accept error");
				printf("Accepted from %s at port %d\n",inet_ntop(AF_INET,&cliaddr.sin_addr,str,sizeof(str)),ntohs(cliaddr.sin_port));
				bool add_res=m_threadpool.addClient(confd,cliaddr);
				if(!add_res)
				{  
					cout<<"Adding new client error"<<endl;
					continue;
				}
				else
				{  
					cout<<"Adding client "<<str<<" at port "<<ntohs(cliaddr.sin_port)<< " success"<<endl;
				}	
				tep.events=EPOLLIN;
				tep.data.fd=confd;
				int res=epoll_ctl(m_epollfd,EPOLL_CTL_ADD,confd,&tep);
				if(res==-1)
				{  
					perr_exit("epoll_ctl_add");
				}
				if(!m_threadpool.append(confd))
				{  
					cout<<"append new client error"<<endl;
					exit(-1);
				}
				//continue;
			}
			//int sockfd=events[i].data.fd;
			//memset(buf,'\0',sizeof(buf));
			//int n=read(sockfd,buf,MAXLINE);
			//if( n==-1)
			//{  
			//	perr_exit("read_error");
			//	deleteClient(sockfd);
			//	int res=epoll_ctl(m_epollfd,EPOLL_CTL_DEL,sockfd,NULL);
			//	if(res==-1)
			//	{  
			//		perr_exit("epoll_ctl_del");
			//	}
			//	printf("client %s at port %d  closed connection\n",inet_ntop(AF_INET,&clientSockets[sockfd]->sin_addr,str,sizeof(str)),ntohs(clientSockets[sockfd]->sin_port));
			//}
			//else if(n>0)
			//{  
			//	processMessage(sockfd,buf);
			//}
		}
		nready=0;
	//sleep(  10);
	}
}

//bool server::addClient(int sockfd,struct sockaddr_in& addr)
//{  
//	if(clientSockets.find(sockfd)==clientSockets.end())
//	{  
//		clientSockets[sockfd]=&addr;
//		return true;
//	}
//	else
//	{  
//		cout<<"this client has existed"<<endl;;
//		return false;
//	}
//}

//void server::deleteClient(int sockfd)
//{  
//	delete clientSockets[sockfd];
//	clientSockets.erase(sockfd);
//	int res=epoll_ctl(m_epollfd,EPOLL_CTL_DEL,sockfd,NULL);
//	if( res<0)
//	{  
//		perr_exit("epol_ctl_del error");
//	}
//	close(sockfd);
//}

//void server::processMessage(int sockfd,char* mess)
//{  
//	char info_host[INET_ADDRSTRLEN];
//	inet_ntop(AF_INET,&clientSockets[sockfd]->sin_addr,info_host,sizeof(info_host));
//	char send_mess[MAXLINE]="received from ";
//	strcat(send_mess,info_host);
//	strcat(send_mess," at port ");
//	char s[MAXLINE];
//	sprintf(s,"%d",ntohs(clientSockets[sockfd]->sin_port));
//	strcat(send_mess,s);
//	strcat(send_mess," :");
//	strcat(send_mess,mess);
//	//cout<<"send-message is:  "<<send_mess<<endl;
//	sendToAllUsers(send_mess);
//}

//void server::sendToAllUsers(char* mess)
//{
//	cout<<mess;
//	map<int,struct sockaddr_in*>::const_iterator i;
//	int count=0;
//	for(i=clientSockets.begin();i!=clientSockets.end();i++)
//	{  	
//		count++;
//		if(write((i->first),mess,strlen(mess))<0)
//		{  
//			deleteClient(i->first);
//			cout<<"send error"<<endl;
//			continue;
//		}
//		cout<<"send to user "<<count<<endl;
//	}
//}

server::~server()
{  
//	map<int,struct sockaddr_in*>::const_iterator i;
//	for(i=clientSockets.begin();i!=clientSockets.end();i++)
//	{  
//		delete i->second;
//		close(i->first);
//	}	
	close(m_listenfd);
	close(m_epollfd);
}

int main(  )
{  
	server s;
	s.server_run();
	return 0;
}
