# TinyWebServer

A high-performance, multi-threaded HTTP server written in modern C++ for Linux.

## Features
- Epoll-based Reactor pattern (ET + non-blocking I/O)
- Thread pool for request handling
- Full HTTP/1.1 request parsing
- Static file serving with MIME type detection
- Configurable via `server.conf`
- Structured logging

## Build
```bash
mkdir build && cd build
cmake ..
make
./tiny_webserver