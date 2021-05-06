#include "config.h"

Config::Config() {}
Config::~Config() {}

const int Config::DEFAULT_SERVER_PORT=8888;//服务器端口
const int Config::DEFAULT_CLIENT_PORT=8080;//客户端端口
const int Config::MAX_CONNECTION_NUM=500;//最大链接数
const int Config::BUF_LENGTH=1024;//最大缓冲区长度
const string Config::DEFAULT_SERVER_ADDR_STR="127.0.0.1";//服务器监控地址
const string Config::OK_STATUS_STR="200 OK";//响应
const string Config::ERROR_METHOD_STR="501 Not Implemented";//响应
const string Config::DENY_STR="403 Forbidden";//响应
const string Config::NOT_FOUND_STR="404 Not Found";//响应
const string Config::DEFAULT_HTTP_VERSION_STR="HTTP/1.1 ";//协议版本
const string Config::SERVER_STR="Server: lijun_http1.1(windows)";//服务器名
