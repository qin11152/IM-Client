#pragma once

#include <QWidget>
#include <QRegularExpression>

namespace Ui 
{ 
    class RegisterWidget; 
};

class RegisterWidget : public QWidget
{
    Q_OBJECT

public:
    RegisterWidget(QWidget *parent = Q_NULLPTR);
    ~RegisterWidget();
    RegisterWidget(const RegisterWidget&) = delete;
    RegisterWidget& operator=(const RegisterWidget&) = delete;
    void cleanLineEdit();

signals:
    void signalShowLoginInWidget();
    //������Ϣ�ź�
    void signalSendMsg(std::string msg);

protected:
    void closeEvent(QCloseEvent* event)override;

private slots:
    void onNickLineEditEditingFinished(const QString& text);
    //��һ�������꣬����Ƿ����������ʽҪ������ڶ��в�Ϊ�գ���Ҫ�鿴�����Ƿ���ͬ
    void onfirstPasswordLineEditEditingFinished(const QString&text);
    //�ڶ��������꣬����Ƿ�����һ��
    void onsecondPasswordLineEditEditingFinished(const QString&text);
    //���ע�ᰴť������Ƿ��ܹ�ע�ᣬ���оͷ���
    void onRegisterClicked();
    //�յ���������������Ϣ�Ĵ���
    void onMsgHandle(const QString& msg);
    //���ӷ�����ʧ��ʱ����ʾ
    void onSignalConnectedFailed();

private:
    Ui::RegisterWidget *ui;

    //��ʼ���ۺ�������
    void initConnect();
    //��ʼ������
    void initUi();
    char m_cBuffer[1024]{ 0 };
    int m_endPosOfBuffer{ -1 };
};
