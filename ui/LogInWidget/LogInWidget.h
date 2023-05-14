﻿#pragma once

#include "ui_LogInWidget.h"
#include "ui/RegisterWidget/RegisterWidget.h"
#include "module/DataBaseDelegate/DataBaseDelegate.h"

#include <QWidget>

class LogInWidget : public QWidget
{
    Q_OBJECT

public:
    LogInWidget(QWidget* parent = Q_NULLPTR);
    ~LogInWidget() override;
    //无需拷贝构造函数和赋值构造函数
    LogInWidget(const LogInWidget&) = delete;
    LogInWidget& operator=(const LogInWidget&) = delete;

signals:
    void signalLoginWidSendMsg(std::string msg);

private slots:
    //注册按钮点击响应事件
    void onRegisterButtonClicked();
    //登录按钮点击响应事件
    void onLogInButtonClicked();
    //注册完成了
    void onRegisterFinished();
    //登录信号返回
    void onSignalLoginResultRecv(const QString& msg);

private:
    Ui::LogInWidget ui;
    RegisterWidget* m_ptrRegisterWidget{nullptr}; //注册界面指针
    QString m_strUserId{""}; //存储登录用户id
    //TCPThread* m_ptrTCPThread{ nullptr };
    bool isLogin{ false };

    void initUi();
    //初始化信号槽的连接
    void initConnection();
};

