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

