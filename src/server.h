#include <QThread>
#include <winsock2.h>
#include <string>
#include <QObject>
using namespace std;

namespace server {
class AcceptThread;
}

class SendThread : public QThread
{//发送客户端数据的线程
    Q_OBJECT

private:
    int clientPort;//客户端接口
    int fileLength=0;//传送的数据大小
    SOCKET acceptSocket;//通信的socket
    sockaddr_in clientSock;//客户端IP地址
    string mainPath;//主目录
    string clientAddrStr;//客户端IP地址（点分10进制）
    string method;//请求命令
    string url;//URL
    string statusCode;//404 NOT FOUND etc...

public:
    explicit SendThread(QObject *parent=0);//显式构造函数
    ~SendThread();//析构函数
    void run();//启动线程
    void setAcceptSocket(SOCKET s);//设置通信的socket
    void setClientAddr(sockaddr_in addr);//设置客户端IP
    void setMainPath(string str);//设置主目录

signals:
    void respondDone(QStringList msg);
};

class ListenThread : public QThread
{//接收客户端连接的线程
    Q_OBJECT

private:
    string mainPath;//主目录
    SOCKET listenSocket;//监听socket
    SendThread *sdThread;//发送信息线程的指针

public:
    explicit ListenThread(QObject *parent=0);//显式构造函数
    ~ListenThread();//析构函数
    void setListenSocket(SOCKET s);//设置监听队列
    void setMainPath(string str);//设置主目录
    void run();//启动线程

signals:
    void acceptOK(QStringList msg);//可以接受信息
    void acceptDone();//接收完成
};


