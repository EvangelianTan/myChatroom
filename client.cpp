#include "client.h"
using namespace std;
int m_sockfd;
client::client()
{  
	m_port=9007;
	m_sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(m_sockfd==-1)
	{  
		perror_exit("sockfd creat error:");
	}
	bzero(&m_address,sizeof(m_address));
	m_address.sin_family=AF_INET;
	inet_pton(AF_INET,"192.168.80.138",&m_address.sin_addr);
	//m_address.sin_addr.s_addr=inet_addr(host);
	m_address.sin_port=htons(m_port);
	
}

struct argum
{  
	int m_sockfd;
	char* buf;
};

static void *doit(void *vptr)
{  
	char buf[MAXLINE];
	int flags=fcntl(m_sockfd,F_GETFL);
	flags|=O_NONBLOCK;
	if(fcntl(m_sockfd,F_SETFL,flags)==-1)
		perror("fcntl");

	while(1)
	{  
		int n=read(m_sockfd,buf,MAXLINE);
		//cout<<buf<<endl;
		if(n==0||n==-1)
		{  	
			continue;
		}
		else if(n>0)
		{	
			if(write(STDOUT_FILENO,buf,n)<0)
			{
				perror("write to stdout error");
				cout<<endl;
				pthread_exit((void*)2);
			}
		}
		else
		{  
			perror("read error");
			exit(1);
		}
	}
}



void client::Connect(  )
{  
	if(connect(m_sockfd,(struct sockaddr*)& m_address,sizeof(m_address))<0)
		perror_exit("connect error");
	else
	{		
		cout<<"connect: "<<m_sockfd<<endl;
		
		cout<<"connect successes"<<endl;
		if(pthread_create(&tid,NULL,doit,NULL))
		{  
			perror_exit("pthread_create error");
		}
		
	}

	cout<<"please input message:"<<endl;
	const char* r="exit";
	while(1)
	{  		
		if(fgets(message,MAXLINE,stdin)!=NULL)
		{  
				if(strlen(message)>0)
				{  	
					if(write(m_sockfd,message,strlen(message))<0)
						perror_exit("write_error");
					
					if(strncasecmp(message,r,4)==0)
					{  
						close(m_sockfd);
						cout<<"equal"<<endl;
						pthread_cancel(tid);
						exit(1);
					}

				}
		}
	}
}

client::~client()
{  
	delete message;
	write(m_sockfd,"exit",strlen("exit"));
	close(m_sockfd);
	bzero(&m_address,sizeof(m_address));
	pthread_cancel(tid);
}

int main()
{  
	client c;
	c.Connect();
	return 0;

}
