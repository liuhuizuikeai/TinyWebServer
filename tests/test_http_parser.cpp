// 文件路径: test/test_http_parser.cpp

#include <gtest/gtest.h>
#include <string>
#include <memory>

// 假设这是你的头文件路径
#include "HttpRequestParser.h" 
#include "HttpRequest.h" // 假设解析结果存在这个类中

// 使用智能指针管理资源
using namespace std;

class HttpParserTest : public ::testing::Test {
protected:
    // 每个测试用例执行前都会调用，用于初始化
    void SetUp() override {
        parser = make_unique<HttpRequestParser>();
    }

    // 每个测试用例执行后调用
    void TearDown() override {
        parser.reset();
    }

    unique_ptr<HttpRequestParser> parser;
};

// ---------------------------------------------------
// 测试用例 1: 正常 GET 请求解析
// 描述：验证标准 HTTP/1.1 GET 请求的解析
// ---------------------------------------------------
TEST_F(HttpParserTest, ParseGetRequest) {
    string request = 
        "GET /index.html HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "User-Agent: TestClient\r\n"
        "\r\n";

    HttpRequest req;
    HttpParseResult result = parser->Parse(request, &req);

    EXPECT_EQ(result, HttpParseResult::OK);
    EXPECT_EQ(req.method(), "GET");
    EXPECT_EQ(req.path(), "/index.html");
    EXPECT_EQ(req.version(), "HTTP/1.1");
    EXPECT_EQ(req.GetHeader("Host"), "www.example.com");
}

// ---------------------------------------------------
// 测试用例 2: 正常 POST 请求解析 (含 Body)
// 描述：验证带有 Content-Length 的 POST 请求
// ---------------------------------------------------
TEST_F(HttpParserTest, ParsePostRequestWithBody) {
    string request = 
        "POST /submit HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 11\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "\r\n"
        "key=value";

    HttpRequest req;
    HttpParseResult result = parser->Parse(request, &req);

    EXPECT_EQ(result, HttpParseResult::OK);
    EXPECT_EQ(req.method(), "POST");
    EXPECT_EQ(req.path(), "/submit");
    EXPECT_EQ(req.GetBody(), "key=value");
}

// ---------------------------------------------------
// 测试用例 3: 边界情况 - 畸形请求行 (无效方法)
// 描述：验证非法的 HTTP 方法是否被正确拒绝
// ---------------------------------------------------
TEST_F(HttpParserTest, ParseInvalidMethod) {
    string request = 
        "GARbage /test HTTP/1.1\r\n"
        "Host: badhost\r\n"
        "\r\n";

    HttpRequest req;
    HttpParseResult result = parser->Parse(request, &req);

    // 通常返回 BAD_REQUEST 或类似错误
    EXPECT_EQ(result, HttpParseResult::BAD_REQUEST); 
}

// ---------------------------------------------------
// 测试用例 4: 边界情况 - 超长 URL (模糊测试)
// 描述：验证长 URL 是否会导致崩溃或溢出
// ---------------------------------------------------
TEST_F(HttpParserTest, ParseLongUrl) {
    string long_path(2048, 'a'); // 构造一个超长路径
    string request = 
        "GET /" + long_path + " HTTP/1.1\r\n"
        "Host: longurl\r\n"
        "\r\n";

    HttpRequest req;
    HttpParseResult result = parser->Parse(request, &req);

    // 即使服务器不支持这么长，也不应该崩溃
    EXPECT_NE(result, HttpParseResult::PARSING_ERROR); 
}

// ---------------------------------------------------
// 测试用例 5: 边界情况 - 不完整的请求 (分块接收模拟)
// 描述：模拟网络分包，验证解析器能否处理部分数据
// ---------------------------------------------------
TEST_F(HttpParserTest, ParseIncompleteRequest) {
    string partial_request = "GET /partial HTTP/1.1\r\nHost: test\r\n";
    
    HttpRequest req;
    // 假设 Parse 返回 NEED_MORE_DATA 表示需要更多数据
    HttpParseResult result = parser->Parse(partial_request, &req);

    EXPECT_EQ(result, HttpParseResult::NEED_MORE_DATA);
}

// ---------------------------------------------------
// 测试用例 6: 边界情况 - 错误的 HTTP 版本
// 描述：验证非标准版本号的容错性
// ---------------------------------------------------
TEST_F(HttpParserTest, ParseUnsupportedHttpVersion) {
    string request = 
        "GET /old HTTP/0.9\r\n"
        "Host: old\r\n"
        "\r\n";

    HttpRequest req;
    HttpParseResult result = parser->Parse(request, &req);

    // 服务器通常会拒绝 HTTP/0.9 或标记为错误
    EXPECT_EQ(result, HttpParseResult::BAD_REQUEST); 
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}