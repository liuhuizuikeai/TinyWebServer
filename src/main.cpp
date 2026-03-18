// src/main.cpp
#include "HttpServer.h"
#include "Logger.h"
#include <csignal>
#include <iostream>
#include <memory>
#include <cstdlib>
#include <stdexcept>
using namespace std;

// 全局服务器指针，用于信号处理
unique_ptr<HttpServer> g_server;

/**
 * @brief 信号处理函数：捕获 SIGINT (Ctrl+C) 和 SIGTERM
 */
void signal_handler(int sig) {
    string sig_name;
    switch (sig) {
        case SIGINT:  sig_name = "SIGINT"; break;
        case SIGTERM: sig_name = "SIGTERM"; break;
        default:      sig_name = "UNKNOWN";
    }
    LOG("Received signal " + sig_name + ", shutting down server gracefully...");
    if (g_server) {
        g_server->stop();
    }
}

/**
 * @brief 打印使用帮助
 */
void print_usage(const char* prog_name) {
    cerr << "Usage: " << prog_name << " [OPTIONS]\n"
              << "Options:\n"
              << "  -p, --port PORT      Set listening port (default: 4000)\n"
              << "  -r, --root PATH      Set document root (default: ./www)\n"
              << "  -h, --help           Show this help\n";
}

/**
 * @brief 简单命令行参数解析（可后续替换为 getopt 或配置文件）
 */
struct ServerConfig {
    int port = 4000;
    string doc_root = "./www";
};

ServerConfig parse_args(int argc, char* argv[]) {
    ServerConfig config;
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-p" || arg == "--port") {
            if (i + 1 < argc) {
                config.port = atoi(argv[++i]);
            } else {
                cerr << "Error: missing port value after " << arg << endl;
                exit(1);
            }
        } else if (arg == "-r" || arg == "--root") {
            if (i + 1 < argc) {
                config.doc_root = argv[++i];
            } else {
                cerr << "Error: missing root path after " << arg << endl;
                exit(1);
            }
        } else if (arg == "-h" || arg == "--help") {
            print_usage(argv[0]);
            exit(0);
        } else {
            cerr << "Unknown argument: " << arg << endl;
            print_usage(argv[0]);
            exit(1);
        }
    }
    return config;
}

/**
 * @brief 主函数
 */
int main(int argc, char* argv[]) {
    // 解析命令行参数
    ServerConfig config = parse_args(argc, argv);

    // 注册信号处理器
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    try {
        // 创建并启动服务器
        g_server = make_unique<HttpServer>(config.port, config.doc_root);
        LOG("Starting TinyWebServer on port " + to_string(config.port));
        LOG("Document root: " + config.doc_root);

        g_server->start(); // 阻塞运行

        LOG("Server stopped.");
    } catch (const exception& e) {
        cerr << "Fatal error: " << e.what() << endl;
        return 1;
    }

    return 0;
}