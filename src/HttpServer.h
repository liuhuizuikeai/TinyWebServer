// HttpServer.h
#pragma once

#include <string>
#include <memory>
#include "EpollReactor.h"
#include "ThreadPool.h"
#include <stdexcept>
class HttpServer {
public:
    HttpServer(int port, const std::string& doc_root);
    ~HttpServer();

    void start();
    void stop();

private:
    void handle_request(int client_fd);

    int port_;
    std::string doc_root_;
    std::unique_ptr<EpollReactor> reactor_;
    ThreadPool pool_;
};