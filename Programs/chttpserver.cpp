#include "chttpserver.h"
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>

CHttpServer::CHttpServer(int port, int timeout) : m_port(port), m_timeout(timeout), m_exit(false)
{
    // 创建监听socket
    m_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listen_fd < 0) {
        perror("socket create failed");
        exit(1);
    }

    // 设置socket选项
    int opt = 1;
    setsockopt(m_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 绑定地址
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(m_port);

    if (bind(m_listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(1);
    }

    // 创建epoll实例
    m_epoll_fd = epoll_create1(0);
    if (m_epoll_fd < 0) {
        perror("epoll create failed");
        exit(1);
    }
}

CHttpServer::~CHttpServer()
{
    m_exit = true;
    close(m_listen_fd);
    close(m_epoll_fd);
    m_ThreadListen.join();
}

void CHttpServer::start()
{
    // 开始监听
    if (listen(m_listen_fd, SOMAXCONN) < 0) {
        perror("监听失败");
        exit(1);
    }

    // 将监听socket添加到epoll
    add_fd(m_listen_fd);

    create_work_thread();
}

void CHttpServer::stop()
{
    m_exit = true;
}

void CHttpServer::handle_accept()
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    int client_fd = accept(m_listen_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        perror("accept failed");
        return;
    }
    std::string client_ip = std::string(inet_ntoa(client_addr.sin_addr));
    std::cout << "新客户端连接: " << client_ip << std::endl;
    m_clients[client_fd] = client_ip;
    m_client_timeout[client_fd] = m_timeout;
    // 设置非阻塞并添加到epoll
    set_nonblocking(client_fd);
    add_fd(client_fd);
}

void CHttpServer::handle_read(int fd)
{
    char buffer[MAX_BUFFER];
    int n = read(fd, buffer, MAX_BUFFER);
    std::string received_data;
    
    if (n <= 0) {
        remove_fd(fd);
        return;
    }

    received_data.assign(buffer, n);
    std::cout << "\nReceived data: \n" << received_data << std::endl;
    // 这里应该添加HTTP请求解析逻辑

    
    // 简单起见，这里直接返回一个基本的HTTP响应
    const char* response = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/html\r\n"
                          "Connection: close\r\n"
                          "\r\n"
                          "<html><body><h1>Hello, World!</h1></body></html>";
    
    write(fd, response, strlen(response));
    remove_fd(fd);
}

void CHttpServer::handle_write(int fd)
{
    // 实现写事件处理逻辑
}

void CHttpServer::add_fd(int fd)
{
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET; // 边缘触发模式
    ev.data.fd = fd;
    epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &ev);
}

void CHttpServer::remove_fd(int fd)
{
    epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    std::cout << "移除客户端: " << m_clients[fd] << std::endl;
    m_clients.erase(fd);
    m_client_timeout.erase(fd);
    close(fd);
}

void CHttpServer::set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}   

void CHttpServer::keep_alive()
{
    while (!m_exit)
    {
        for (auto it = m_client_timeout.begin(); it != m_client_timeout.end();)
        {
            if (it->second <= 0)
            {
                int fd = it->first;
                remove_fd(fd);
                m_clients.erase(fd);
                it = m_client_timeout.erase(it);
            }
            else
            {
                --(it->second);
                ++it;
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void CHttpServer::create_work_thread()
{
    m_ThreadListen = std::thread([this]() {
        listen_();
    });
    m_ThreadKeepAlive = std::thread([this]() {
        keep_alive();
    });
}

void CHttpServer::listen_()
{
    while (!m_exit) {
            int nfds = epoll_wait(m_epoll_fd, m_events, MAX_EVENTS, -1);
            std::cout << "等待连接" << std::endl;
            if (nfds < 0) {
                if (errno == EINTR) continue;
                perror("epoll_wait失败");
                break;
            }

            for (int i = 0; i < nfds; i++) {
                int fd = m_events[i].data.fd;
                uint32_t events = m_events[i].events;
                
                if (fd == m_listen_fd) {
                    handle_accept();
                } else {
                    if (events & EPOLLIN) {
                        handle_read(fd);
                    }
                    if (events & EPOLLOUT) {
                        handle_write(fd);
                    }
                    if (events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                        // 处理连接关闭、挂起或错误
                        remove_fd(fd);
                    }
                }
            }
        }
}
