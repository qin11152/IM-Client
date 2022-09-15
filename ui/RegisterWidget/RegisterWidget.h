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
    //发送消息信号
    void signalSendMsg(std::string msg);

protected:
    void closeEvent(QCloseEvent* event)override;

private slots:
    void onNickLineEditEditingFinished(const QString& text);
    //第一次输入完，检查是否符合正则表达式要求，如果第二行不为空，还要查看两次是否相同
    void onfirstPasswordLineEditEditingFinished(const QString&text);
    //第二行输入完，检查是否两次一致
    void onsecondPasswordLineEditEditingFinished(const QString&text);
    //点击注册按钮，检查是否能够注册，不行就返回
    void onRegisterClicked();
    //收到服务器传来的消息的处理
    void onMsgHandle(const QString& msg);
    //连接服务器失败时的提示
    void onSignalConnectedFailed();

private:
    Ui::RegisterWidget *ui;

    //初始化槽函数连接
    void initConnect();
    //初始化界面
    void initUi();
    char m_cBuffer[1024]{ 0 };
    int m_endPosOfBuffer{ -1 };
};
