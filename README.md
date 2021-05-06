---
title: 简易多线程带图形界面的Web服务器
date: 2020-11-23 18:59:23
tags: [c++&Qt]
---



### 简介

我使用Qt 和C++ 多线程库实现的简单web 服务器，能解析HTTP 请求报文并构造相应的响应报文，
具有一定的异常处理功能。

### 开发环境

操作系统：	Windows 10 Pro 64-bit
编译器：	    MinGW 7.3.0 32-bit
开发平台：	Qt 5.12.9 

<!--more-->

### 具备功能

- 可配置Web服务器的监听地址、监听端口和主目录；
- 能够多线程处理多个请求。当一个客户（浏览器,输入URL：http://202.103.2.3/index.html）连接时创建一个连接套接字；
- 从连接套接字接收http请求报文，并根据请求报文的确定用户请求的网页文件；
- 从服务器的文件系统获得请求的文件。 创建一个由请求的文件组成的http响应报文。；
- 经TCP连接向请求的浏览器发送响应，浏览器可以正确显示网页的内容；
- 能够传输包含多媒体（如图片，视频）的网页给客户端，并能在客户端正确显示；
- 在服务器端的屏幕上输出请求的来源（IP地址、端口号和HTTP请求命令行）；
- 在服务器端的屏幕上能够输出对每一个请求处理的结果；
- 对于无法成功定位文件的请求，根据错误原因，作相应错误提示，并具备一定的异常情况处理能力。
- 构造了简洁实用的GUI界面，使得用户操作更清晰简明。

### 总体设计

![](/images/p5.jpg)

图中，主线程用来运行GUI界面模块，GUI模块开始监听后，将会创建一个监听子线程Listenthread，监听子线程完成接受来自客户端的连接后，将会发送AcceptOK，然后创建一个发送信息的子线程Sendthread，利用已经连接的socket与客户端进行通信。另外，send线程会发送响应报头和请求文件传输给客户端，并返回RespondOK，之后，send线程会更新BufInfo，并发出BufInfo已经被更新的信号，GUI接受到被更新的信号后会从BufInfo中读取信息来更新GUI的显示。

这样实现不仅有利于模块的管理和修改，还有利于GUI界面信息和服务端和客户端通信信息的统一，实现实时显示。

### GUI模块

用户GUI界面的模块主要包括显示数据模块、修改主目录模块、修改监听地址模块、修改监听端口模块和开始结束模块，其主要功能为与用户进行交互并将交互数据发送给核心线程进行处理。显示数据模块的主要功能是显示由send线程发送的缓冲区数据，每当send缓冲区发生改变时，send线程发送信号给图形用户界面以更新其内容。

#### 具体实现

主要就是使用Qt提供的一些API，对按钮添加事件的监听器，对不同的事件作出不同的响应，以及日志的显示。这里日志的显示参考了后端框架Flask的日志格式，满足了实验对于日志输出的要求。实验中主要遇到的难点就是对于Qt线程机制的理解。由于在Qt中始终要有一个主线程用于事件的循环，如果将web服务器放到Qt的主线程中执行，就会发生阻塞，所有的点击事件以及日志显示全部都被阻塞，所以需要使用QThread库，将原来的server包装成一个继承自QObject的子类，并定义run(),stop()等方法。在使用的时候将其放入workerthread中执行。这样才解决了GUI阻塞的问题。

GUI主要是通过创建一个UI类（MainWindow类）实现的，下面简单介绍一下这个类的结构，具体的功能见注释。

```c++
#include <QMainWindow>
#include <winsock2.h>
#include "server.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{//基于Qt5.12的ui类
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    WSADATA mWsaData;
    SOCKET listenSocket=INVALID_SOCKET;//监听的socket
    ListenThread *lsThread;//服务器指针

public:
    explicit MainWindow(QWidget *parent = 0);//显式构造函数
    ~MainWindow();//析构函数
    void closeEvent(QCloseEvent *event);//关闭所有活动，退出

private slots:
    void on_startPushBotton_clicked();//开始监听按钮
    void on_endPushBotton_clicked();//结束监听按钮
    void on_pathPushBotton_clicked();//打开主目录
    void on_actionAbout_triggered();//校交1802李俊信息
};
```

由于GUI模块不是本次实验的重点，具体各个部分实现的细节就不详细展开了。具体的实现细节可以参考源码的注释

### server模块

Server模块主要由listen模块和send模块实现。

**Listen模块主要实现的功能：**

1. 后台启动Listen线程(run函数部分)与客户端连接，防止阻塞UI主线程
2. 监听端口，如果某个客户程序要求连接的时候，会新开启新线程SendThread与客户端通信
3. 如果不是监听端口或者监听地址，直接断开连接，发送拒绝请求页面。如果是监听端口和监听地址，开启新线程与客户端通信。
4. 新线程会建立并返回一个新的SOCKET来与对方通信，新建的SOCKET与原来的SOCKET有相同的特性，包括端口号。原来的SOCKET继续等待其他的连接请求。而新生成的SOCKET才是与客户端通信的实际SOCKET。
5. 接受新线程发来的消息，更新UI缓冲区。

**Send模块主要实现的功能：**

①在已经和客户端通信连接的基础上，根据客户端请求报文设置服务器响应报文头部。

②如果URL正常能够GET到服务器的资源，发送200 OK的响应报文，并发送对应的资源文件给客户端，如果不能定位，则根据错误类型发送响应的响应报文。

③将本次通信的信息发送给UI的BUF缓冲区，用于UI主线程更新界面信息。

#### 具体实现

Server模块是本次实验的核心模块，它完成了端口的监听及与客户端进行通信的功能。

下面简单说明server模块中每个部分的关键点：

①**多线程的创建以及管理**

在查询了相关资料之后发现如果使用thread库的话，相关函数的参数传递还是很麻烦，所以最终使用了thread库，用来创建多线程。由于创建的线程在detach之后就没有什么非常方便的办法来判断是否结束,所以就包装了一个类listenThread,用其中的一个字段来判断会话是否结束.同时维护了一个listenThread的列表对象,用于存放当前还在处理的会话,根据列表的长度对请求进行限制,如果达到了设置的上限,就阻塞循环,直到有线程结束为止.这样就实现了对线程数量的限制，为了防止阻塞监听线程，还创建了sendThread用于解析请求报文并发送响应报文和对应的请求文件给客户端。

**②解析请求资源的路径**

由于请求的路径中可能存在[/.]以及[/..],所以需要对这些符号进行适当的处理,虽然说直接打开没什么大问题,但是可能会导致用户可以请求到设置的根目录之上的文件,进而对web服务的隐私产生影响,所以需要对请求路径进行处理,如果是违规请求就发送501 Not Implement响应.在具体解析的时候不断地对’/./’以及’/../’进行处理，直到URL中没有为止 。

③**响应报文的构建和发送**

主要还是根据预先写好的一些主流文件类型对应的content-type,找到请求文件对应的content-type,再进行发送.在发送文件的时候,有可能会出现由于文件过大,导致无法装入内存的问题,所以就没有直接将整个文件装入内存再发送,而是配合利用C中fread和socket库的send函数,类似于流式文件,每次发送1024个字节。

④**请求报文解析**

主要就是利用map对象,实现字符串到字符串的映射,用于存储请求中的header字段

```c++
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
```

可见server模块做的工作基本包含了本次实验所有的关键点：首先，创建服务器套接字并绑定套接字和服务器地址信息。然后监听端口，如果某个客户程序要求连接的时候，会新开一个线程sendthread与之进行通信。当连接数量达到最大时，再来的连接请求都将被拒绝。如果有请求，则后台启动listen线程(run函数部分)与客户端连接，防止阻塞UI主线程。如果不是监听端口或者监听地址，直接断开连接，发送拒绝请求页面。如果是监听端口和监听地址，开启新线程与客户端通信，新线程会建立并返回一个新的SOCKET来与对方通信，新建的SOCKET与原来的SOCKET有相同的特性，包括端口号。原来的SOCKET继续等待其他的连接请求。而新生成的SOCKET才是与客户端通信的实际SOCKET。最后接受新线程发来的消息，更新UI缓冲区。

### Config模块

Config模块主要用于设置关键的全局变量。如设置服务器端口、客户端端口、服务器监控地址、最大缓冲区长度、HTTP版本号等等。

```c++
#include <string>
using namespace std;

class Config
{
public:
    static const int DEFAULT_SERVER_PORT;       //服务器默认端口号
    static const int DEFAULT_CLIENT_PORT;       //客户端默认监听端口
    static const int MAX_CONNECTION_NUM;        //最大连接数
    static const int BUF_LENGTH;                //缓冲区大小
    static const string DEFAULT_SERVER_ADDR_STR; //服务器默认监听地址
    static const string OK_STATUS_STR;          //客户端请求成功
    static const string ERROR_METHOD_STR;        //客户端语法错误
    static const string DENY_STR;               //该客户端不在服务范围
    static const string NOT_FOUND_STR;          //文件不存在
    static const string DEFAULT_HTTP_VERSION_STR;       //HTTP版本
    static const string SERVER_STR;
private:
    Config();
    ~Config();
};
```

### 功能测试

#### 测试环境

操作系统：     Windows 10 Pro 64-bit

浏览器：    Google Chrome 86.0.4240.75（64-bit）

#### 测试功能

①配置监听地址，监听窗口及监听主目录，点击START，开始监听。

![](/images/p6.jpg)

②访问服务器主页http://127.0.0.1:8888/index.html，查看服务器是否能够响应客户端的请求。

![](/images/p7.jpg)

如上图所示，服务器能够响应客户端的请求，并定位相应的html文件。这也说明服务器可以正常在监听端口上进行监听并且能够收到客户端请求时能创建连接套接字。

③查看与客户端通信时客户端的请求报文。

```http
GET /index.html HTTP/1.1
Host: 127.0.0.1:8888
Connection: keep-alive
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.75 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9
Sec-Fetch-Site: none
Sec-Fetch-Mode: navigate
Sec-Fetch-User: ?1
Sec-Fetch-Dest: document
Accept-Encoding: gzip, deflate, br
Accept-Language: zh-CN,zh;q=0.9,en-CN;q=0.8,en;q=0.7
```

可以看出，请求报文包括请求命令行为GET，请求文件为/index.html，HTTP版本号为HTTP/1.1。除此之外，还包括Host，浏览器版本，接受语言等等信息

④本服务器支持多种类型文件的输出，如txt文本，jpg/jpeg图片，html文件，MP3音频，MP4视频等等。在这里就不展示啦。

⑤查看文件响应的来源，并在服务器端的屏幕上输出每个请求的来源（IP地址、端口号和HTTP请求命令行），具体如下图所示

![](/images/p8.jpg)

### 注意事项

输入的URL不能有中文，服务器系统没有对中文编码进行识别

### 参考文献

[1] Computer Networking A Top-Down Approach(7th Edition)

[2] 池义勇,李巧云,杨新峰;[Web服务器CGI程序的设计与实现](http://www.cnki.com.cn/Article/CJFDTOTAL-DLXZ199705006.htm)[J];电脑学习;2010年05期

[3] 马毅;[轻量级Web服务器的实现与应用](http://cdmd.cnki.com.cn/Article/CDMD-10697-2008076853.htm)[D];西北大学;2015年

[4] 郭静盈;[基于IPv6的HTTP Server的研究与实现](http://cdmd.cnki.com.cn/Article/CDMD-10703-2009135294.htm)[D];西安建筑科技大学;2009年

[5] 王雷,TCP/IP网络编程基础教程,北京理工大学出版社,2017.02,第4页

[6] 潘伟编著,计算机网络 理论与实验,厦门大学出版社,2013.12,第145页

[7] 计算机网络:自顶向下方法(原书第 7 版)/(美)詹姆斯.F.库罗斯,(美) 基思.W.罗斯著;陈明泽.—北京:机械工业出版社,2018