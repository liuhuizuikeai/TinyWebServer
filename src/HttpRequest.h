// HttpRequest.h
#pragma once

#include <string>
#include <unordered_map>
#include <stdexcept>

class HttpRequest {
public:
    HttpRequest();  // 默认构造函数
    bool parse(const char* start, const char* end);

    const std::string& method() const { return method_; }
    const std::string& path() const { return path_; }
    const std::string& version() const { return version_; }
    const std::string& get_header(const std::string& key) const;

private:
    std::string method_;
    std::string path_;
    std::string version_;
    std::unordered_map<std::string, std::string> headers_;
};