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
#include <map>

namespace Ui 
{ 
    class ChatWidget; 
};

enum TrayIconState {
    NormalState,
    BlinkState
};

//聊天界面中的stackwidget中的通用界面id，比如添加好友
enum ChatStackedWidgetCommonWidget
{
    AddFriendWid = 19999,
    EmptyWid
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
    void onSignalSwitchLastChatWidget(int type);
    //朋友列表被点击后
    void onSignalFriendListClicked(int iId);
    //底部托盘被点击后
    void onSignalTrayTriggered(QSystemTrayIcon::ActivationReason reason);
    //收到好友聊天消息后
    void onSignalSingleChatMessage(const QString& chatMessage);
    //收到qml页面同意添加好友的请求
    void onSignalAgreeAddFriend(QString friendName);
    //收到好友列表后
    void onSignalFriendList(const QString& friendList);

private:
    void initUi();
    void initConnect();
    void initData();
    void getLastChatListFromDB();
    void notifyServerOnline();
    //获取好友列表
    void getFriendList();
    //void getFriendListFromServer();

private:
    Ui::ChatWidget *ui;
    QObject* m_ptrLastChatQMLRoot{ nullptr }; //上次聊天qml的根对象
    QObject* m_ptrFriendListQMLRoot{ nullptr };//好友列表qml的根对象
    QTimer* m_ptrNullMessageTimer{ nullptr };   //空的聊天界面
    QQuickWidget* m_ptrLastChatWidget{ nullptr }; //上次聊天列表界面
    QQuickWidget* m_ptrFriendListWidget{ nullptr }; //搜索好友左侧界面
    QQuickWidget* m_ptrNewFriendAndAreadyAddWidget{ nullptr };  //显示已添加和新好友请求的界面
    QSystemTrayIcon* m_ptrTrayIcon{ nullptr };  //托盘图标
    //这个用户的id
    int m_iId{ -1 };
    std::map<std::string, std::string> m_mapUserInfo;     //存储用户id和对应的用户名
    int m_iTrayState{ NormalState };        //托盘图标的状态，是否为闪烁
};
