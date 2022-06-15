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
    //������ע��ҳ��ָ��
    if (m_ptrRegisterWidget != nullptr)
    {
        delete m_ptrRegisterWidget;
        m_ptrRegisterWidget = nullptr;
    }
}

void LogInWidget::onRegisterButtonClicked()
{
    showMinimized();
    //���û�������ʹ���ע����沢��ʾ
    if (m_ptrRegisterWidget == nullptr)
    {
        m_ptrRegisterWidget = new RegisterWidget();
        //TODO �ƶ�������λ��
        m_ptrRegisterWidget->showNormal();
    }
    //��������ֱ����ʾ
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
    //�Ƚ�������
    LoginInReplyData loginReplyData(msg.toStdString());
    //�����¼�ɹ�
    if (loginReplyData.m_bLoginInResult)
    {
        //�򿪶�Ӧ���������
        ChatWidget* ptrChatWidget = new ChatWidget(m_strUserId);
        //�ý�������,һ�������
        hide();
        close();
        //���������ʾ
        ptrChatWidget->show();
    }
    //TODO��½ʧ�ܴ���
}

void LogInWidget::initConnection()
{
    connect(ui.logInButton, &QPushButton::clicked, this, &LogInWidget::onLogInButtonClicked);
    connect(ui.registerButton, &QPushButton::clicked, this, &LogInWidget::onRegisterButtonClicked);
    connect(m_ptrRegisterWidget, &RegisterWidget::signalShowLoginInWidget, this, &LogInWidget::onRegisterFinished);
    connect(TCPConnect::Instance().get(), &TCPConnect::signalRecvLoginResultMessage, this, &LogInWidget::onSignalLoginResultRecv);
}
