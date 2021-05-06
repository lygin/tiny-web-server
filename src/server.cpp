#include "server.h"
#include "config.h"
#include <QDebug>
#include <sstream>
#include <string>
#include <regex>
#include <QDebug>
#include <windows.h>//系统底层，判断目录是否存在
#include <QTime>
using namespace std;

ListenThread::ListenThread(QObject *parent)
    :QThread(parent)
{//创建监听线程
}
ListenThread::~ListenThread()
{//释放监听线程
    requestInterruption();
    terminate();
    quit();
    wait();
    closesocket(this->listenSocket);
    qDebug()<<"accept thread析构，socket关闭";
    emit acceptDone();
}
void ListenThread::setListenSocket(SOCKET s)
{//设置接受的socket
    this->listenSocket=s;
}
void ListenThread::setMainPath(string str)
{//设置客户端IP
    this->mainPath=str;
}

//运行接收客户端连接的线程
void ListenThread::run()
{
    SOCKET sAccept;
    sockaddr_in clientAddr;
    int iLen=sizeof(clientAddr);
    int i=0;//for debug
    while(!isInterruptionRequested())
    {//没有终止信号请求，一直循环接受消息
        //缓存清零
        memset(&clientAddr,0,sizeof(clientAddr));
        //等待新客户端连接，阻塞函数必须用terminate打断，requestinterruption不管用
        sAccept=accept(this->listenSocket,(sockaddr*)&clientAddr,&iLen);//用于通信的新socket
        if(sAccept==INVALID_SOCKET)
        {
            qDebug()<<"accept error:"<<WSAGetLastError();
            return;
        }
        i++;
        //如果不是监听端口或者监听地址，直接断开连接，发送拒绝请求页面

        //开启新线程与客户端通信
        this->sdThread=new SendThread;
        this->sdThread->setAcceptSocket(sAccept);//设置通信socket
        this->sdThread->setMainPath(this->mainPath);
        this->sdThread->setClientAddr(clientAddr);
        sdThread->start();

        connect(sdThread,&SendThread::respondDone,this,[=](QStringList msg){
            emit acceptOK(msg);
        });

        connect(this,&ListenThread::acceptDone,this,[=](){
            delete sdThread;
            sdThread=nullptr;
        });
    }
}

static bool endWith(const string &fullStr,const string &endStr)
{//辅助函数，确定fullStr以endStr后缀结尾，如xxx.txt
    if(endStr.size()>fullStr.size())
    {
        return false;
    }
    int indexFull=fullStr.size()-1;
    int indexEnd=endStr.size()-1;
    while(indexEnd>=0)
    {
        if(fullStr[indexFull]!=endStr[indexEnd])
        {
            return false;
        }
        indexFull--;
        indexEnd--;
    }
    return true;
}

SendThread::SendThread(QObject *parent)
    :QThread(parent)
{//创建发送线程
}
SendThread::~SendThread()
{//释放发送线程
    requestInterruption();
    terminate();
    quit();
    wait();
    closesocket(acceptSocket);
    qDebug()<<"recv thread 析构，socket关闭";
}
void SendThread::setAcceptSocket(SOCKET s)
{
    acceptSocket=s;
}
void SendThread::setClientAddr(sockaddr_in addr)
{
    clientSock=addr;
}
void SendThread::setMainPath(string str)
{
    mainPath=str;
}

//运行发送给客户端数据的线程
void SendThread::run()
{
    while(!isInterruptionRequested())
    {//没有终止请求
        char recvBuf[Config::BUF_LENGTH];//接收报文缓冲区
        char sendBuf[Config::BUF_LENGTH];//发送报文缓冲区
        string fullPath;//完整路径
        string respondHttpHeader,clientHttpHeader;//HTTP服务器响应报头和客户端请求报头
        string statusCode,firstHeader,typeStr,lengthStr;//返回状态，报头第一行，传输文件类型，传输文件长度
        regex regRequest(R"(([A-Z]+) (.*?) HTTP/\d\.\d)");//用于正则表达式查询
        smatch matchRst;//查询返回
        string strRecv,strSend;//接受到的请求报文和准备发的响应报文
        QStringList msg;//用于更新UI
        clientAddrStr.assign(inet_ntoa(clientSock.sin_addr));//设置客户端地址
        clientPort=clientSock.sin_port;//设置客户端端口

        int rtn;
        int i;
        int bufReadNum;

        DWORD ftyp;

        //缓存清零
        respondHttpHeader="";
        statusCode=Config::OK_STATUS_STR;
        firstHeader="HTTP/1.1 200 OK\r\n";

        memset(recvBuf,0,sizeof(recvBuf));
        rtn=recv(acceptSocket,recvBuf,sizeof(recvBuf),0);
        if(rtn==SOCKET_ERROR)
        {//接收失败
            return;
        }
        if(rtn==0)
        {//对方断开连接
            return;
        }

        //找到HTTP报文头部
        strRecv.assign(recvBuf);//char* to string
        size_t headerEnd=strRecv.find("\r\n\r\n");
        clientHttpHeader=strRecv.substr(0,headerEnd);
        qDebug()<<endl<<"[client http header:]"<<endl<<clientHttpHeader.c_str();

        //正则表达式提取关键字method(GET,POST)，url
        if(regex_search(clientHttpHeader,matchRst,regRequest))
        {
            method=matchRst[1].str();
            url=matchRst[2].str();
        }
        //替换成Windows路径
        for(i=0;i<(int)url.size();i++)
        {
            if(url[i]=='/')
                url[i]='\\';
        }
        fullPath=mainPath+url;//完整路径

        ftyp=GetFileAttributesA(fullPath.c_str());
        if((ftyp&FILE_ATTRIBUTE_DIRECTORY)&&(!INVALID_FILE_ATTRIBUTES))
        {//是一个目录，打开该目录下的索引index.html
            fullPath=fullPath+"\\index.html";
        }

        FILE* infile=fopen(fullPath.c_str(),"rb");


        if(endWith(url,".html")||endWith(url,"htm"))
        {
            typeStr="Content-Type: text/html\r\n";
        }
        else if(endWith(url,".txt"))
        {
            typeStr="Content-Type: text/plain\r\n";
        }
        else if(endWith(url,".jpg"))
        {
            typeStr="Content-Type: image/jpeg\r\n";
        }
        else if(endWith(url,".jpeg")||endWith(url,".png"))
        {
            typeStr="Content-Type: image/"
                    +(url.substr(url.rfind('.')+1))
                    +"\r\n";
        }
        else if(endWith(url,".ico"))
        {
            typeStr="Content-Type: image/x-icon\r\n";
        }
        else if(endWith(url,".mp4"))
        {
            typeStr="Content-Type: video/mp4\r\n";
        }
        else
        {
            fclose(infile);
            //发送自定义501页面
            infile=fopen((mainPath+"\\501.html").c_str(),"rb");
            //infile=fopen("501.html","rb");
            statusCode=Config::ERROR_METHOD_STR;
            firstHeader="HTTP/1.1 501 Not Implemented\r\n";
            typeStr="Content-Type: text/html\r\n";
        }
        if(!infile)
        {//文件不存在
            fclose(infile);
            //发送自定义404页面
            infile=fopen((mainPath+"\\404.html").c_str(),"rb");
            statusCode=Config::NOT_FOUND_STR;
            firstHeader="HTTP/1.1 404 Not Found\r\n";
            typeStr="Content-Type: text/html\r\n";
        }
        //获取文件大小
        fseek(infile,0,SEEK_SET);
        fseek(infile,0,SEEK_END);
        fileLength=ftell(infile);
        //文件指针归位
        fseek(infile,0,SEEK_SET);
        //响应报头
        respondHttpHeader =
                firstHeader
                +"Connection: close\r\n"
                +"Date: " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString() + "\r\n"
                +Config::SERVER_STR+ "\r\n"
                +"Content-Length: "+to_string(fileLength)+"\r\n"
                +typeStr + "\r\n";

        qDebug()<<endl<<"[respond http header:]\n"<<respondHttpHeader.c_str();
        //发送报头
        rtn=send(acceptSocket,
                 respondHttpHeader.c_str(),
                 (int)(respondHttpHeader.length()),
                 0);

        //发送请求的文件        
        while(true)
        {
            //缓存清零
            memset(sendBuf,0,sizeof(sendBuf));
            bufReadNum=fread(sendBuf,1,Config::BUF_LENGTH,infile);//file流读文件数据给sendbuf，每次读1B
            if(SOCKET_ERROR==(send(acceptSocket,sendBuf,bufReadNum,0)))
            {//发送失败
                rtn=SOCKET_ERROR;
                break;
            }
            if(feof(infile))
                break;
        }
        fclose(infile);
        //msg UI信息缓冲区更新
        msg.clear();
        msg<<QDateTime::currentDateTime().toString("yyyy.MM.dd")
        <<QTime::currentTime().toString("hh:mm:ss")
        <<QString::fromStdString(clientAddrStr)
        <<QString::fromStdString(to_string(clientPort))
        <<QString::fromStdString(method)
        <<QString::fromStdString(url);
        if(rtn==SOCKET_ERROR)
        {
            msg<<QString::fromStdString(to_string(WSAGetLastError()));
        }
        else
        {
            if(statusCode == "10053") msg<<QString::fromStdString(Config::OK_STATUS_STR);
            else msg<<QString::fromStdString(statusCode);
        }
        emit respondDone(msg);
        return;
    }
}
