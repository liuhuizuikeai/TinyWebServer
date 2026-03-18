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

##总结
# TinyWebServer

> 一个轻量级、高性能、基于 C++11 的单线程 HTTP/1.1 静态文件服务器，支持多平台编译与部署。  
> 本项目在经典开源项目 [tinyhttpd](https://github.com/EZLippi/Tinyhttpd) 基础上进行了架构重构、安全性增强与功能扩展。

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

---

## 📌 目录

- [项目亮点](#-项目亮点)
- [与 tinyhttpd 的对比与优化](#-与-tinyhttpd-的对比与优化)
- [核心问题与解决方案](#-核心问题与解决方案)
- [快速开始](#-快速开始)
- [目录结构](#-目录结构)
- [配置说明](#-配置说明)
- [未来计划](#-未来计划)
- [许可证](#-许可证)

---

## 🌟 项目亮点

- ✅ **纯 C++11 实现**：无第三方依赖，仅需标准库和 POSIX 接口。
- ✅ **路径安全处理**：防止目录穿越（如 `../` 攻击）。
- ✅ **MIME 类型自动识别**：根据文件扩展名返回正确 `Content-Type`。
- ✅ **优雅错误处理**：404、500 等状态码响应规范。
- ✅ **支持静态资源服务**：HTML、CSS、JS、图片等。
- ✅ **完整 Git 集成**：已上传至 [GitHub 仓库](https://github.com/liuhuizuikeai/TinyWebServer.git)

---

## 🔍 与 tinyhttpd 的对比与优化

| 功能 / 特性 | **tinyhttpd（原始）** | **TinyWebServer（本项目）**    | 优化说明                                       |
|-------------|------------------------|------------------------------|------------------------------------------------|
| **语言**    |       C                |            C++11             | 利用 RAII、智能指针、STL 容器提升安全性与可读性 |
| **路径解析**       | 直接拼接字符串         | 显式映射 + 路径校验          | 避免路径注入风险（如 `/../../../etc/passwd`）   |
| **文件存在性检查** | 未严格验证       | 使用 `stat()` + 错误日志     | 防止因工作目录错误导致 404                      |
| **Content-Type**   | 固定为 `text/plain` | 动态识别扩展名              | 正确渲染 HTML/CSS/JS/图片                    |
| **分支管理** | 无版本控制建议         | 使用 `main` 分支 + `.gitignore` | 符合现代 Git 规范                           |
| **构建系统** | Makefile（简单）       | CMake（跨平台）             | 支持 Linux/macOS/Windows 编译                   |
| **错误页面** | 无                     | 返回 `<h1>404 Not Found</h1>` | 用户友好                                      |
| **工作目录依赖** | 强依赖运行路径     | 支持相对/绝对路径配置         | 解决 `./www` 在 `build/` 下找不到的问题         |

> 💡 **tinyhttpd 主要缺点**：
> - 路径拼接不安全，易受目录遍历攻击。
> - 所有文件返回 `text/plain`，浏览器无法正确解析 HTML。
> - 无错误处理机制，文件不存在时行为未定义。
> - 代码耦合度高，难以扩展。

> ✅ **本项目优化点**：
> - 引入 `get_content_type()` 函数，支持常见 MIME 类型。
> - 明确分离请求解析、路径映射、文件读取逻辑。
> - 添加详细日志（如 `Resolved path: ...`, `File exists? YES/NO`），便于调试。
> - 通过 `stat()` 严格验证文件存在性，避免“假 404”。

---
  ```bash
  git branch -M main      # 重命名本地分支
  git push -u origin main # 推送
