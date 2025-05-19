#ifndef CHTTPSERVER_H
#define CHTTPSERVER_H
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <thread>
#include <iostream>
#include <arpa/inet.h>
#include <map>
#include <sstream>
#include <json/json.h>
#include "Programs/ceventhandler.h"
#include "ceventhandler.h"

class CHttpServer
{
enum {MAX_EVENTS = 10000, MAX_BUFFER = 1024};
public:
    CHttpServer(int port, int timeout);
    ~CHttpServer();
    void start();
    void stop();
private:
    void create_work_thread();
    void keep_alive();
    void listen_();
private:
    bool m_exit;                     // 退出标志
    int m_port;                      // 监听端口
    int m_epoll_fd;                  // epoll文件描述符
    int m_listen_fd;                 // 监听socket
    int m_timeout;                   // 超时时间
    struct epoll_event m_events[MAX_EVENTS]; // epoll事件数组
    void handle_accept();            // 处理新连接
    void handle_read(int fd);        // 处理读事件
    void handle_write(int fd);       // 处理写事件
    void add_fd(int fd);            // 添加文件描述符到epoll
    void remove_fd(int fd);         // 从epoll移除文件描述符
    void set_nonblocking(int fd);   // 设置非阻塞
    std::map<int, std::string> m_clients;
    std::map<int, int> m_client_timeout;
    std::mutex m_mutex;             // 添加互斥锁保护共享资源
    
    std::shared_ptr<CThreadPool> m_ThreadPool;
    std::thread m_ThreadKeepAlive;
    std::thread m_ThreadListen;
    std::shared_ptr<CEventHandler> m_EventHandler;
};

#endif // CHTTPSERVER_H
