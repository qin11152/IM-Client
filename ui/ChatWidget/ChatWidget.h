/************************************************************************/
/*聊天主界面，即用户登录后进行聊天等操作的界面，也是最为重要的界面 
*/
/************************************************************************/

#pragma once

#include "../../module/MyChatMessageQuickWid/MyChatMessageQuickWid.h"
#include <QWidget>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QQuickWidget>

namespace Ui 
{ 
    class ChatWidget; 
};

enum TrayIconState {
    NormalState,
    BlinkState
};

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    ChatWidget(int id,QWidget *parent = Q_NULLPTR);
    ~ChatWidget();
    ChatWidget(const ChatWidget& l) = delete;
    ChatWidget& operator=(const ChatWidget& l) = delete;

protected slots:
    //事件过滤器的重载
    bool eventFilter(QObject* watched, QEvent* event)override;

private slots:
    //文本框输入获得焦点
    void onSignalTextEditIsFocus(bool isFocus);
    //发送消息按钮被点击后
    void onSignalSendMessage();
    void onSignalSwitchFriendListWidget(int type);
    //朋友列表被点击后
    void onSignalFriendListClicked(int iId);
    //底部托盘被点击后
    void onSignalTrayTriggered(QSystemTrayIcon::ActivationReason reason);
    //收到好友聊天消息后
    void onSignalSingleChatMessage(QString& chatMessage);
    void onStateChanged(QQuickWidget::Status status);
    void sceneGraphError(QQuickWindow::SceneGraphError, const QString& message);

private:
    void initUi();
    void initConnect();
    void initData();
    void getLastChatListFromDB();
    void notifyServerOnline();
    //void getFriendListFromServer();

private:
    Ui::ChatWidget *ui;
    QObject* m_ptrFriendListQMLRoot{ nullptr }; //qml的根对象
    QTimer* m_ptrNullMessageTimer{ nullptr };   //空的聊天界面
    QQuickWidget* m_ptrFriendListWidget{ nullptr }; //上次聊天列表界面
    QQuickWidget* m_ptrSearchWidget{ nullptr }; //搜索好友界面
    QSystemTrayIcon* m_ptrTrayIcon{ nullptr };  //托盘图标
    //这个用户的id
    int m_iId{ -1 };

    int m_iTrayState{ NormalState };        //托盘图标的状态，是否为闪烁
};
