#include "LogInWidget.h"
//#include "../../module/TCPConnect/TCPConnect.h"
#include "../../protocol/LoginInJsonData/LoginInJsonData.h"
#include "protocol/LoginInReplyData/LoginInReplyData.h"
#include "module/TCPThread/TCPThread.h"
#include "ChatWidget/ChatWidget.h"
#include <QMessageBox>

LogInWidget::LogInWidget(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    m_ptrRegisterWidget = new RegisterWidget();
    ui.passwordLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    setWindowTitle(QString::fromLocal8Bit("q΢��"));
    TCPThread::get_mutable_instance().start();
    initConnection();
}

LogInWidget::~LogInWidget()
{
    if(!isLogin)
    {
        TCPThread::get_mutable_instance().quit();
    }
    Sleep(100);
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
        //�ƶ�������λ��
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
    protocol::LoginInJsonData loginJsonData("");
    loginJsonData.m_strId = ui.userNameLineEdit->text().toStdString();
    m_strUserId = ui.userNameLineEdit->text();
    loginJsonData.m_strPassword = ui.passwordLineEdit->text().toStdString();
    std::string message = loginJsonData.generateJson();
    TCPThread::get_mutable_instance().sendMessage(message);
    //emit signalLoginWidSendMsg(message);
}

void LogInWidget::onRegisterFinished()
{
    this->showNormal();
    //activateWindow();
}

void LogInWidget::onSignalLoginResultRecv(const QString& msg)
{
    //�Ƚ�������
    protocol::LoginInReplyData loginReplyData(msg.toStdString());
    //�����¼�ɹ�
    if (loginReplyData.m_bLoginInResult)
    {
        //�򿪶�Ӧ���������
        auto ptrChatWidget = new ChatWidget(m_strUserId, QString::fromStdString(loginReplyData.m_strUserName));
        //�ý�������,һ�������
        isLogin = true;
        hide();
        close();
        //���������ʾ
        ptrChatWidget->show();
    }
    else
    //��½ʧ�ܴ���
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("��¼ʧ��"),
                             QString::fromLocal8Bit("����������ٴγ���"),
                             QMessageBox::Ok);
    }
}

void LogInWidget::initConnection()
{
    connect(ui.logInButton, &QPushButton::clicked, this, &LogInWidget::onLogInButtonClicked);
    connect(ui.registerButton, &QPushButton::clicked, this, &LogInWidget::onRegisterButtonClicked);
    connect(m_ptrRegisterWidget, &RegisterWidget::signalShowLoginInWidget, this, &LogInWidget::onRegisterFinished);
    connect(&TCPThread::get_mutable_instance(), &TCPThread::signalRecvLoginResultMessage, this, &LogInWidget::onSignalLoginResultRecv, Qt::QueuedConnection);
}
