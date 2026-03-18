// EpollReactor.cpp
#include "EpollReactor.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <cerrno>
#include <stdexcept>

EpollReactor::EpollReactor()
    : listen_fd_(-1), epoll_fd_(-1), running_(false) {}

EpollReactor::~EpollReactor() {
    stop();
}

void EpollReactor::start(int port, std::function<void(int)> on_accept) {
    on_accept_ = std::move(on_accept);

    // 创建监听 socket
    listen_fd_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (listen_fd_ == -1) {
        perror("socket");
        return;
    }

    int opt = 1;
    setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(listen_fd_, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(listen_fd_);
        listen_fd_ = -1;
        return;
    }

    if (listen(listen_fd_, SOMAXCONN) == -1) {
        perror("listen");
        close(listen_fd_);
        listen_fd_ = -1;
        return;
    }

    // 创建 epoll
    epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd_ == -1) {
        perror("epoll_create1");
        close(listen_fd_);
        listen_fd_ = -1;
        return;
    }

    struct epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd_;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, listen_fd_, &ev) == -1) {
        perror("epoll_ctl listen_fd");
        close(epoll_fd_);
        close(listen_fd_);
        epoll_fd_ = -1;
        listen_fd_ = -1;
        return;
    }

    running_ = true;
    std::cout << "Server listening on port " << port << std::endl;

    // Event loop
    std::vector<epoll_event> events(64);
    while (running_) {
        int nfds = epoll_wait(epoll_fd_, events.data(), static_cast<int>(events.size()), -1);
        if (nfds == -1) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == listen_fd_) {
                // Accept new connection
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept4(listen_fd_, (struct sockaddr*)&client_addr,
                                        &client_len, SOCK_NONBLOCK);
                if (client_fd == -1) {
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                        perror("accept4");
                    }
                    continue;
                }

                if (on_accept_) {
                    on_accept_(client_fd);
                }
            }
        }
    }
}

void EpollReactor::stop() {
    running_ = false;
    if (epoll_fd_ != -1) {
        close(epoll_fd_);
        epoll_fd_ = -1;
    }
    if (listen_fd_ != -1) {
        close(listen_fd_);
        listen_fd_ = -1;
    }
}