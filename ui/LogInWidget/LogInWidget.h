#pragma once

#include "ui_LogInWidget.h"
#include "../RegisterWidget/RegisterWidget.h"
#include "../../module/DataBaseDelegate/DataBaseDelegate.h"
#include <QWidget>

class LogInWidget : public QWidget
{
    Q_OBJECT

public:
    LogInWidget(QWidget* parent = Q_NULLPTR);
    ~LogInWidget() override;
    //���追�����캯���͸�ֵ���캯��
    LogInWidget(const LogInWidget&) = delete;
    LogInWidget& operator=(const LogInWidget&) = delete;

signals:
    void signalLoginWidSendMsg(std::string msg);

private slots:
    //ע�ᰴť�����Ӧ�¼�
    void onRegisterButtonClicked();
    //��¼��ť�����Ӧ�¼�
    void onLogInButtonClicked();
    //ע�������
    void onRegisterFinished();
    //��¼�źŷ���
    void onSignalLoginResultRecv(const QString& msg);

private:
    Ui::LogInWidget ui;
    RegisterWidget* m_ptrRegisterWidget{nullptr}; //ע�����ָ��
    QString m_strUserId{""}; //�洢��¼�û�id
    //TCPThread* m_ptrTCPThread{ nullptr };
    bool isLogin{ false };

    void initUi();
    //��ʼ���źŲ۵�����
    void initConnection();
};

