//单例日志
// Logger.h
#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <mutex>

class Logger {
public:
    static Logger& instance() {
        static Logger inst;
        return inst;
    }

    void log(const std::string& msg) {
        std::lock_guard<std::mutex> lock(mtx_);
        std::cout << "[LOG] " << msg << std::endl;
        // 可扩展：写入文件
    }

private:
    Logger() = default;
    std::mutex mtx_;
};
#define LOG(msg) Logger::instance().log(msg)