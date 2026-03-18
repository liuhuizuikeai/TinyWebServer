cmake_minimum_required(VERSION 3.10)
project(TinyWebServer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Release)

add_executable(tiny_webserver
    src/main.cpp
    src/HttpServer.cpp
    src/HttpRequest.cpp
    src/HttpResponse.cpp
    src/ThreadPool.cpp
    src/EpollReactor.cpp
    src/Logger.cpp
)

target_include_directories(tiny_webserver PRIVATE src)
target_link_libraries(tiny_webserver pthread)