/************************************************************************/
/*聊天主界面，即用户登录后进行聊天等操作的界面，也是最为重要的界面 
*/
/************************************************************************/

#pragma once

#include "../../module/MyChatMessageQuickWid/MyChatMessageQuickWid.h"
#include "module/MyDefine.h"
#include <QWidget>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QQuickWidget>
#include <map>
#include <unordered_set>

namespace Ui 
{ 
    class ChatWidget; 
};

enum TrayIconState 
{
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
    ChatWidget(QString id,QWidget *parent = Q_NULLPTR);
    ~ChatWidget();
    ChatWidget(const ChatWidget& l) = delete;
    ChatWidget& operator=(const ChatWidget& l) = delete;

protected slots:
    //事件过滤器的重载
    bool eventFilter(QObject* watched, QEvent* event)override;

public:
    //像上次聊天列表中添加一个新的，一般是添加了新的好友的时候调用
    void onSignalAdd2LastChat(const MyFriendInfoWithFirstC& friendInfo);

private slots:
    //文本框输入获得焦点
    void onSignalTextEditIsFocus(bool isFocus);
    //发送消息按钮被点击后
    void onSignalSendMessage();
    //上次聊天列表被点击后
    void onSignalLastChatItemClicked(QString strId);
    //好友列表被点击
    void onSignalFriendListItemClicked(QString strId);
    //底部托盘被点击后
    void onSignalTrayTriggered(QSystemTrayIcon::ActivationReason reason);
    //收到好友聊天消息后
    void onSignalSingleChatMessage(const QString& chatMessage);
    //搜索输入框失去焦点后
    void onSignalSearchTextLoseFocus(bool isFocus);
    //点击侧边栏会话按钮
    void onSignalChatBtn();
    //点击左侧通讯录按钮
    void onSignalFriendListBtn();
    //点击侧边栏添加好友按钮
    void onSignalAddFriendBtn();
    //收到好友列表消息
    void onSignalRecvFriendList(const QString& friendList);
    //manager处理好友列表完毕
    void initFriendList();
    //根据id初始化此id对应的聊天界面
    void initChatMessageWidAcordId(QString strId);
    //初始化上次聊天列表
    void initLastChatList();
    //初始化聊天界面
    void initAllChatWid();

private:
    void initUi();
    void initConnect();
    void initData();
    //void notifyServerOnline();

private:
    Ui::ChatWidget *ui;
    QObject* m_ptrLastChatQMLRoot{ nullptr }; //上次聊天qml的根对象
    QObject* m_ptrFriendListQMLRoot{ nullptr };//好友列表qml的根对象
    QObject* m_ptrAddFriendQMLRoot{ nullptr };  //添加好友qml界面的根对象
    QTimer* m_ptrNullMessageTimer{ nullptr };   //空的聊天界面
    QQuickWidget* m_ptrLastChatWidget{ nullptr }; //上次聊天列表界面
    QQuickWidget* m_ptrFriendListWidget{ nullptr }; //好友列表界面
    QQuickWidget* m_ptrNewFriendAndAreadyAddWidget{ nullptr };  //显示已添加和新好友请求的界面
    QQuickWidget* m_ptrSearchFriendList{ nullptr };//搜索好友时显示的界面
    QQuickWidget* m_ptrEmptyWid{ nullptr };     //空的界面
    QSystemTrayIcon* m_ptrTrayIcon{ nullptr };  //托盘图标
    //这个用户的id
    QString m_strUserId{ "" };
    int m_iTrayState{ NormalState };        //托盘图标的状态，是否为闪烁

        //存储好友信息，带首字母
    std::vector<MyFriendInfoWithFirstC> m_vecFriendInfoWithC;
    std::map<QString, int> m_mapUserInfo;     //存储好友id和该id在vec中对应的位置，以便查找信息
    std::vector<QString> m_vecLastChatFriend;        //上次聊天页面里的好友
};
