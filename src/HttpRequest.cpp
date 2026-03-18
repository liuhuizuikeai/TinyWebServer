// HttpRequest.cpp
#include "HttpRequest.h"
#include <string_view>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <vector>
#include <sstream>


HttpRequest::HttpRequest() {
    // 初始化成员变量
    method_ = "";
    path_ = "";
    version_ = "";
}

bool HttpRequest::parse(const char* start, const char* end) {
    std::string buffer(start, end);  // ← 用 string 代替 string_view

    size_t pos1 = buffer.find("\r\n");
    if (pos1 == std::string::npos) return false;

    std::string request_line = buffer.substr(0, pos1);

    size_t pos2 = request_line.find(' ');
    if (pos2 == std::string::npos) return false;

    method_ = request_line.substr(0, pos2);

    size_t pos3 = request_line.find(' ', pos2 + 1);
    if (pos3 == std::string::npos) return false;

    path_ = request_line.substr(pos2 + 1, pos3 - pos2 - 1);

    version_ = request_line.substr(pos3 + 1);

    // 解析 headers...
    size_t header_start = pos1 + 2;
    while (header_start < buffer.size()) {
        size_t line_end = buffer.find("\r\n", header_start);
        if (line_end == std::string::npos) break;

        if (line_end == header_start) break; // 空行

        std::string header_line = buffer.substr(header_start, line_end - header_start);
        size_t colon = header_line.find(':');
        if (colon != std::string::npos) {
            std::string key(header_line.substr(0, colon));
            std::string value(header_line.substr(colon + 1));
            while (!value.empty() && std::isspace(static_cast<unsigned char>(value[0]))) {
                value.erase(0, 1);
            }
            for (char& c : key) c = std::tolower(static_cast<unsigned char>(c));
            headers_[key] = value;
        }

        header_start = line_end + 2;
    }

    return true;
}

const std::string& HttpRequest::get_header(const std::string& key) const {
    static const std::string empty;
    auto it = headers_.find(key);
    if (it != headers_.end()) {
        return it->second;
    }
    return empty;
}