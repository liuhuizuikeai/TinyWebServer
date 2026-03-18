// HttpServer.cpp
#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
HttpServer::HttpServer(int port, const std::string& doc_root)
    : port_(port), doc_root_(doc_root), reactor_(std::make_unique<EpollReactor>()), pool_(4) {}

HttpServer::~HttpServer() = default;

void HttpServer::start() {
    reactor_->start(port_, [this](int fd) {
        pool_.enqueue([this, fd]() {
            handle_request(fd);
        });
    });
}

void HttpServer::stop() {
    reactor_->stop();
    pool_.stop();
}

void HttpServer::handle_request(int client_fd) {
    char buffer[4096];
    ssize_t n = read(client_fd, buffer, sizeof(buffer) - 1);
    if (n <= 0) {
        close(client_fd);
        return;
    }
    buffer[n] = '\0';

    HttpRequest req;
    HttpResponse resp;

    if (req.parse(buffer, buffer + n)) {
        std::string path = req.path();
        // 支持根路径 / 映射到 index.html
        if (path == "/" || path == "/index.html") {
    path = "./www/index.html";
        } else 
        {
    // 确保路径以 / 开头，去掉第一个 /
            if (path[0] == '/') 
            {
            path = "./www" + path.substr(1);
            }else 
            {
            path = "./www/" + path;
            }
        }

        // 添加调试日志
        std::cout << "Request path: " << req.path() << std::endl;
        std::cout << "Resolved path: " << path << std::endl;

        struct stat st;  //  声明 st
        if (stat(path.c_str(), &st) != 0) {
             std::cout << "File exists? NO" << std::endl;
            resp.set_status(HttpResponse::NOT_FOUND);
            resp.set_body("<h1>404 Not Found</h1>");
        } else {
            std::cout << "File exists? YES" << std::endl;
            std::ifstream file(path);
            std::stringstream ss;
            ss << file.rdbuf();
            std::string content = ss.str();
            
            std::cout << "Content-Type: " << get_content_type(path) << std::endl;

            resp.set_status(HttpResponse::OK);
            resp.set_content_type(get_content_type(path));
            resp.set_body(content);
        }
    } else {
        resp.set_status(HttpResponse::BAD_REQUEST);
    }

    resp.send(client_fd);
    close(client_fd);
}