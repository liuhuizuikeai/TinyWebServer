//工具函数（trim，get_mime_type 等）
// src/utils.h
#pragma once

#include <string>
#include <string_view>

namespace utils {

/**
 * @brief 根据文件扩展名返回 MIME 类型
 * @param filename 文件名或路径（如 "/index.html"）
 * @return 对应的 MIME 类型字符串
 */
std::string get_mime_type(const std::string& filename);

/**
 * @brief 去除字符串首尾空白字符（空格、制表符、换行）
 * @param s 输入字符串
 * @return 修剪后的字符串
 */
std::string trim(const std::string& s);

/**
 * @brief 安全校验请求路径，防止路径遍历攻击（如 "../etc/passwd"）
 * @param path 原始请求路径（如 "/static/../../secret.txt"）
 * @return 是否合法（true 表示安全）
 *
 * 规则：
 * - 不允许包含 ".."
 * - 必须以 '/' 开头
 * - 不包含空字节或控制字符
 */
bool is_path_safe(const std::string& path);

/**
 * @brief 读取整个文件内容到字符串（适用于小文件）
 * @param filepath 文件绝对路径
 * @param content 输出参数，存储文件内容
 * @return 是否成功读取
 */
bool read_file(const std::string& filepath, std::string& content);

} // namespace utils