# myChatroom
小型聊天室，支持多个客户端，服务器将收到的消息发往各个客户端。
采用线程池和epoll(ET)事件处理模式，
#threadpool.h server.h server.cpp 服务器为连接成功的每个客户端都创建一个线程，使用epoll监听客户端连接，将其加入客户队列中，线程从队列中抢占任务，从当前客户端读取消息并将消息发往各个客户端。
#client.h client.cpp 客户端连接服务器端成功后，主线程读取输入的字符串发往服务器端，同时创建一个线程用于读取服务器端发来的消息并输出显示。
#locker.h 定义互斥锁，信号量等
目前存在的问题：在客户端关闭之后，服务器端依然向此客户端发送消息
