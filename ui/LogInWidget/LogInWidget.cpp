#include "LogInWidget.h"
#include "../../module/TCPConnect/TCPConnect.h"
#include "../../protocol/LoginInJsonData/LoginInJsonData.h"
#include "protocol/LoginInReplyData/LoginInReplyData.h"
#include "ChatWidget/ChatWidget.h"

LogInWidget::LogInWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    m_ptrRegisterWidget = new RegisterWidget();
    ui.passwordLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    initConnection();
}

LogInWidget::~LogInWidget()
{
    //析构掉注册页面指针
    if (m_ptrRegisterWidget != nullptr)
    {
        delete m_ptrRegisterWidget;
        m_ptrRegisterWidget = nullptr;
    }
}

void LogInWidget::onRegisterButtonClicked()
{
    showMinimized();
    //如果没创建，就创建注册界面并显示
    if (m_ptrRegisterWidget == nullptr)
    {
        m_ptrRegisterWidget = new RegisterWidget();
        //TODO 移动到中心位置
        m_ptrRegisterWidget->showNormal();
    }
    //创建过就直接显示
    else
    {
        m_ptrRegisterWidget->cleanLineEdit();
        m_ptrRegisterWidget->showNormal();
    }
}

void LogInWidget::onLogInButtonClicked()
{
    if (ui.passwordLineEdit->text().isEmpty() || ui.userNameLineEdit->text().isEmpty())
    {
        return;
    }
    else
    {
        LoginInJsonData loginJsonData("");
        loginJsonData.m_strId = ui.userNameLineEdit->text().toStdString();
        m_strUserId = ui.userNameLineEdit->text();
        loginJsonData.m_strPassword = ui.passwordLineEdit->text().toStdString();
        std::string message = loginJsonData.generateJson();
        TCPConnect::Instance()->sendMessage(message);
    }
}

void LogInWidget::onRegisterFinished()
{
    this->showNormal();
    //activateWindow();
}

void LogInWidget::onSignalLoginResultRecv(const QString& msg)
{
    //先解析数据
    LoginInReplyData loginReplyData(msg.toStdString());
    //如果登录成功
    if (loginReplyData.m_bLoginInResult)
    {
        //打开对应的聊天界面
        ChatWidget* ptrChatWidget = new ChatWidget(m_strUserId);
        //该界面隐藏,一会后析构
        hide();
        close();
        //聊天界面显示
        ptrChatWidget->show();
    }
    //TODO登陆失败处理
}

void LogInWidget::initConnection()
{
    connect(ui.logInButton, &QPushButton::clicked, this, &LogInWidget::onLogInButtonClicked);
    connect(ui.registerButton, &QPushButton::clicked, this, &LogInWidget::onRegisterButtonClicked);
    connect(m_ptrRegisterWidget, &RegisterWidget::signalShowLoginInWidget, this, &LogInWidget::onRegisterFinished);
    connect(TCPConnect::Instance().get(), &TCPConnect::signalRecvLoginResultMessage, this, &LogInWidget::onSignalLoginResultRecv);
}
