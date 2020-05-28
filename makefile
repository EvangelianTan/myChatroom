server:locker.h threadpool.h server.h server.cpp
	g++ -g  $^ -o $@ -lpthread

client: client.h client.cpp
	g++ -g  $^ -o $@ -lpthread
clear:
	rm -r server
	rm -r client
	
