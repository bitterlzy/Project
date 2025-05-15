#include "chttpserver.h"
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>

CHttpServer::CHttpServer(int port, int timeout) : m_port(port), m_timeout(timeout), m_exit(false)
{
    m_EventHandler = std::make_shared<CEventHandler>();
    m_EventHandler->init();
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

    // 解析HTTP请求头
    std::istringstream request_stream(received_data);
    std::string request_line;
    std::getline(request_stream, request_line);
    
    // 解析请求方法、路径和HTTP版本
    std::string method, path, version;
    std::istringstream request_line_stream(request_line);
    request_line_stream >> method >> path >> version;

    // 读取请求头
    std::map<std::string, std::string> headers;
    std::string header_line;
    while (std::getline(request_stream, header_line) && header_line != "\r") {
        size_t colon_pos = header_line.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = header_line.substr(0, colon_pos);
            std::string value = header_line.substr(colon_pos + 2); // 跳过": "
            if (!value.empty() && value.back() == '\r') {
                value.pop_back();
            }
            headers[key] = value;
        }
    }

    // 读取请求体（JSON数据）
    std::string body;
    std::string line;
    while (std::getline(request_stream, line)) {
        body += line + "\n";
    }

    // 解析JSON数据
    Json::Value json_data;
    Json::CharReaderBuilder reader;
    Json::String errs;
    std::istringstream body_stream(body);
    try {
        if (!body.empty()) {
            bool parse_success = Json::parseFromStream(reader, body_stream, &json_data, &errs);
            if (!parse_success) {
                std::cerr << "JSON解析错误: " << errs << std::endl;
                const char* error_response = "HTTP/1.1 400 Bad Request\r\n"
                                           "Content-Type: application/json\r\n"
                                           "Connection: close\r\n"
                                           "\r\n"
                                           "{\"error\": \"Invalid JSON format\"}";
                write(fd, error_response, strlen(error_response));
                remove_fd(fd);
                return;
            }
            std::cout << "解析的JSON数据: " << json_data.toStyledString() << std::endl;
            
            // 读取method字段
            if (json_data.isMember("method")) {
                std::string method_value = json_data["method"].asString();
                std::cout << "接收到的method: " << method_value << std::endl;
                
                // 根据method字段处理不同的请求
                if (method_value == "test_service") {
                    // 调用事件处理器中的test_service事件
                    if (m_EventHandler) {
                        // 假设m_EventHandler有一个执行事件的方法
                        m_EventHandler->dispatchEvent("test_service");
                    }
                }
                // 可以添加更多method的处理逻辑
            } else {
                std::cout << "JSON数据中没有method字段" << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "JSON解析异常: " << e.what() << std::endl;
        const char* error_response = "HTTP/1.1 400 Bad Request\r\n"
                                   "Content-Type: application/json\r\n"
                                   "Connection: close\r\n"
                                   "\r\n"
                                   "{\"error\": \"Invalid JSON format\"}";
        write(fd, error_response, strlen(error_response));
        remove_fd(fd);
        return;
    }

    // 构建JSON响应
    Json::Value response_json;
    response_json["status"] = "success";
    response_json["message"] = "Request processed successfully";
    response_json["received_data"] = json_data;

    // 发送JSON响应
    Json::StreamWriterBuilder writer;
    writer["indentation"] = "";  // 不需要缩进
    std::string response_body = Json::writeString(writer, response_json);
    std::string response = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: application/json\r\n"
                          "Content-Length: " + std::to_string(response_body.length()) + "\r\n"
                          "Connection: close\r\n"
                          "\r\n" +
                          response_body;
    
    write(fd, response.c_str(), response.length());
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
