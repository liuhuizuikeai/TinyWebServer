// HttpResponse.cpp
#include "HttpResponse.h"
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <cctype>
#include <filesystem>


HttpResponse::HttpResponse()
    : status_code_(OK), content_type_("text/plain") {}

void HttpResponse::set_status(StatusCode code) {
    status_code_ = code;
}

void HttpResponse::set_content_type(const std::string& type) {
    content_type_ = type;
}

void HttpResponse::set_body(const std::string& body) {
    body_ = body;
}
// HttpResponse.cpp
// HttpResponse.h
std::string get_content_type(const std::string& path);

// HttpResponse.cpp
std::string get_content_type(const std::string& path) {
       size_t last_dot = path.find_last_of(".");
    if (last_dot == std::string::npos) return "application/octet-stream";

    std::string ext = path.substr(last_dot);
    if (ext == ".html" || ext == ".htm") return "text/html";
    if (ext == ".css") return "text/css";
    if (ext == ".js") return "application/javascript";
    if (ext == ".png") return "image/png";
    if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
    if (ext == ".gif") return "image/gif";
    return "application/octet-stream";
}
std::string HttpResponse::build_response() const {
    std::ostringstream oss;
    oss << "HTTP/1.1 " << status_code_ << " ";
    switch (status_code_) {
        case OK: oss << "200 OK"; break;
        case BAD_REQUEST: oss << "400 Bad Request"; break;
        case NOT_FOUND: oss << "404 Not Found"; break;
        default: oss << "500 Internal Server Error";
    }
    oss << "\r\n";
    oss << "Content-Type: " << content_type_ << "\r\n";
    oss << "Content-Length: " << body_.size() << "\r\n";
    oss << "Connection: close\r\n\r\n";
    oss << body_;
    return oss.str();
}

void HttpResponse::send(int fd) {
    std::string response = build_response();
    write(fd, response.c_str(), response.size());
}