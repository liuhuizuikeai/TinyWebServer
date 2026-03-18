// HttpResponse.h
#pragma once

#include <string>
#include <unordered_map>
#include <iostream>
#include <stdexcept>
class HttpResponse {
public:
    enum StatusCode {
        OK = 200,
        BAD_REQUEST = 400,
        NOT_FOUND = 404
    };

    HttpResponse();

    void set_status(StatusCode code);
    void set_content_type(const std::string& type);
    void set_body(const std::string& body);
    void send(int fd);
 
private:
    std::string build_response() const;
    // HttpResponse.h

    StatusCode status_code_;
    std::string content_type_;
    std::string body_;
};
std::string get_content_type(const std::string& path);