// EpollReactor.h
#pragma once

#include <functional>
#include <vector>
#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
class EpollReactor {
public:
    EpollReactor();
    ~EpollReactor();

    // 启动监听：port + 新连接回调（传入 client_fd）
    void start(int port, std::function<void(int)> on_accept);

    // 停止 reactor（关闭监听 socket，退出 loop）
    void stop();

private:
    int listen_fd_;
    int epoll_fd_;
    bool running_;
    std::function<void(int)> on_accept_;
};