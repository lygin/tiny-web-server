/********************************************************************************
** Form generated from reading UI file 'ui.ui'
**
** Created by: Qt User Interface Compiler version 5.12.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UI_H
#define UI_UI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionAbout;
    QAction *actionExit;
    QWidget *centralWidget;
    QLabel *addrLabel;
    QPushButton *pathPushBotton;
    QLabel *maxConnectionLabel;
    QSpinBox *portSpinBox;
    QLineEdit *pathLineEdit;
    QPushButton *endPushBotton;
    QSpinBox *maxConnectionSpinBox;
    QLineEdit *addrLineEdit;
    QPushButton *startPushBotton;
    QLabel *portLabel;
    QLabel *pathLabel;
    QTableWidget *showTableWidget;
    QMenuBar *menuBar;
    QMenu *menuAbout;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(730, 651);
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        addrLabel = new QLabel(centralWidget);
        addrLabel->setObjectName(QString::fromUtf8("addrLabel"));
        addrLabel->setGeometry(QRect(50, 20, 141, 20));
        pathPushBotton = new QPushButton(centralWidget);
        pathPushBotton->setObjectName(QString::fromUtf8("pathPushBotton"));
        pathPushBotton->setGeometry(QRect(630, 120, 25, 21));
        pathPushBotton->setMinimumSize(QSize(25, 0));
        pathPushBotton->setMaximumSize(QSize(50, 16777215));
        maxConnectionLabel = new QLabel(centralWidget);
        maxConnectionLabel->setObjectName(QString::fromUtf8("maxConnectionLabel"));
        maxConnectionLabel->setGeometry(QRect(50, 80, 241, 20));
        portSpinBox = new QSpinBox(centralWidget);
        portSpinBox->setObjectName(QString::fromUtf8("portSpinBox"));
        portSpinBox->setGeometry(QRect(300, 50, 81, 21));
        portSpinBox->setMinimum(-1);
        portSpinBox->setMaximum(65535);
        portSpinBox->setValue(0);
        pathLineEdit = new QLineEdit(centralWidget);
        pathLineEdit->setObjectName(QString::fromUtf8("pathLineEdit"));
        pathLineEdit->setGeometry(QRect(100, 119, 501, 21));
        endPushBotton = new QPushButton(centralWidget);
        endPushBotton->setObjectName(QString::fromUtf8("endPushBotton"));
        endPushBotton->setGeometry(QRect(510, 70, 171, 41));
        maxConnectionSpinBox = new QSpinBox(centralWidget);
        maxConnectionSpinBox->setObjectName(QString::fromUtf8("maxConnectionSpinBox"));
        maxConnectionSpinBox->setGeometry(QRect(300, 80, 81, 20));
        maxConnectionSpinBox->setMaximum(65535);
        maxConnectionSpinBox->setValue(100);
        addrLineEdit = new QLineEdit(centralWidget);
        addrLineEdit->setObjectName(QString::fromUtf8("addrLineEdit"));
        addrLineEdit->setGeometry(QRect(300, 20, 131, 20));
        startPushBotton = new QPushButton(centralWidget);
        startPushBotton->setObjectName(QString::fromUtf8("startPushBotton"));
        startPushBotton->setGeometry(QRect(510, 20, 171, 41));
        portLabel = new QLabel(centralWidget);
        portLabel->setObjectName(QString::fromUtf8("portLabel"));
        portLabel->setGeometry(QRect(50, 50, 151, 21));
        pathLabel = new QLabel(centralWidget);
        pathLabel->setObjectName(QString::fromUtf8("pathLabel"));
        pathLabel->setGeometry(QRect(50, 120, 51, 20));
        showTableWidget = new QTableWidget(centralWidget);
        showTableWidget->setObjectName(QString::fromUtf8("showTableWidget"));
        showTableWidget->setGeometry(QRect(50, 160, 631, 431));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 730, 26));
        menuAbout = new QMenu(menuBar);
        menuAbout->setObjectName(QString::fromUtf8("menuAbout"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuAbout->menuAction());
        menuAbout->addAction(actionAbout);
        menuAbout->addAction(actionExit);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        actionAbout->setText(QApplication::translate("MainWindow", "Lygin", nullptr));
        actionExit->setText(QApplication::translate("MainWindow", "Exit", nullptr));
        addrLabel->setText(QApplication::translate("MainWindow", "Listening address:", nullptr));
        pathPushBotton->setText(QApplication::translate("MainWindow", "..", nullptr));
        maxConnectionLabel->setText(QApplication::translate("MainWindow", "Maximum number of connections:", nullptr));
        endPushBotton->setText(QApplication::translate("MainWindow", "STOP", nullptr));
        startPushBotton->setText(QApplication::translate("MainWindow", "START", nullptr));
        portLabel->setText(QApplication::translate("MainWindow", "Listening port:", nullptr));
        pathLabel->setText(QApplication::translate("MainWindow", "PATH:", nullptr));
        menuAbout->setTitle(QApplication::translate("MainWindow", "\346\240\241\344\272\2441802\346\235\216\344\277\212", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UI_H
