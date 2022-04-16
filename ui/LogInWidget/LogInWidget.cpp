#include "LogInWidget.h"
#include "../../module/TCPConnect/TCPConnect.h"
#include "../../protocol/LoginInJsonData/LoginInJsonData.h"

LogInWidget::LogInWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    m_ptrRegisterWidget = new RegisterWidget();
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
    //TODO 打开对应的聊天界面
}

void LogInWidget::initConnection()
{
    connect(ui.logInButton, &QPushButton::clicked, this, &LogInWidget::onLogInButtonClicked);
    connect(ui.registerButton, &QPushButton::clicked, this, &LogInWidget::onRegisterButtonClicked);
    connect(m_ptrRegisterWidget, &RegisterWidget::signalShowLoginInWidget, this, &LogInWidget::onRegisterFinished);
    connect(TCPConnect::Instance().get(), &TCPConnect::signalRecvLoginResultMessage, this, &LogInWidget::onSignalLoginResultRecv);
}
