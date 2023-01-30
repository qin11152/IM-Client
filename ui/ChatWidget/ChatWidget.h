/************************************************************************/
/*聊天主界面，即用户登录后进行聊天等操作的界面，也是最为重要的界面 
*/
/************************************************************************/

#pragma once

#include "module/stdafx.h"
#include "ui/AddFriendWidget/AddFriendWidget.h"
#include "ui/ProfileImagePreview/ProfileImagePreview.h"
#include "ui/MyChatMessageQuickWid/MyChatMessageQuickWid.h"
#include "ui/ChatWidget/FriendListModel/MyFriendListModel.h"
#include "ui/ChatWidget/FriendListModel/MyFriendListSortModel.h"

#include <QTimer>
#include <QWidget>
#include <QQuickWidget>
#include <QSqlDatabase>
#include <QSystemTrayIcon>


namespace Ui 
{ 
	class ChatWidget; 
};


/**
 * brief: 聊天界面中的几个widget，包括空的界面，添加好友的界面和聊天界面.
 */
enum ChatStackedWidgetCommonWidget
{
	AddFriendWid = 19999,
	EmptyWid,
	ChatWid,
};

class ChatWidget : public QWidget
{
	Q_OBJECT

public:
	ChatWidget(QString id,QString name,QWidget *parent = Q_NULLPTR);
	virtual ~ChatWidget()override;
	ChatWidget(const ChatWidget& l) = delete;
	ChatWidget& operator=(const ChatWidget& l) = delete;
	MyChatMessageQuickWid* getChatMsgWidAcordId(QString id);

	//像上次聊天列表中添加一个新的，一般是添加了新的好友的时候调用
	void onSignalAdd2LastChat(const MyFriendInfoWithFirstC& friendInfo);

protected slots:
	//事件过滤器的重载
	bool eventFilter(QObject* watched, QEvent* event)override;

private slots:
	//manager处理好友列表完毕
	void initFriendList();
	/**
	 * brief.初始化此id对应的聊天界面，如果有十条就添加十条聊天记录，没有就有几条添加几条
	 * 
	 * \param lastChatInfo：姓名和id
	 */
	void initChatMessageWidAcordId(const MyLastChatFriendInfo& lastChatInfo);
	//初始化上次聊天列表
	void initLastChatList();
	//初始化聊天界面
	void initAllChatWid();
	//初始化添加好友界面
	void initAddFriendWid();

	//搜索输入框失去焦点后
	void onSignalSearchTextLoseFocus(bool isFocus);

	/**
	 * brief：搜索消息框中的内容发生了变化.
	 * 
	 * \param searchMsg：搜索的内容
	 */
	void onSignalSearchTextChange(const QString& searchMsg);

	//发送消息按钮被点击后
	void onSignalSendMessage();
	//上次聊天列表被点击后
	void onSignalLastChatItemClicked(const QString strId, const QString strName);
	//好友列表被点击
	void onSignalFriendListItemClicked(QString strId,QString name);
	//底部托盘被点击后
	void onSignalTrayTriggered(QSystemTrayIcon::ActivationReason reason);

	//收到好友聊天消息后
	void onSignalSingleChatMessage(const QString& chatMessage);
	//收到好友列表消息
	void onSignalRecvFriendList(const QString& friendList);
	//有好友同意了好友请求
	void onSignalBecomeFriend(const MyFriendInfoWithFirstC& friendInfo);
	//向好友列表中添加好友
	void onAddFriendIntoList(const MyFriendInfoWithFirstC& friendInfo);

	//点击侧边栏会话按钮
	void onSignalChatBtn();
	//点击左侧通讯录按钮
	void onSignalFriendListBtn();
	//点击侧边栏添加好友按钮
	void onSignalAddFriendBtn();
	//点击侧边栏头像按钮
	void onSignalSideBarProfileImageBtn();

	//和某个好友的聊天页面要求刷新界面，也就是要求增加聊天记录
	void onSignalUpdateChatMessage(const QString id);
	//设置红色提示显示并传递正确的数量
	void onSignalSetRedRectangleShow(const QString& id)const;
	//点击后取消红色消息提示框
	void onSignalHideRedRectangleInLastChat(const QString id);

	//按照首字母的英文对好友列表排序
	void onSortFriendList();
	//刷新界面中的好友列表
	void onUpdateFriendListUI()const;
	//QML界面通知上次聊天界面需要写入数据库
	void onSignalNeedUpdateLastChat()const;
	//QML页面通知打开头像预览
	void onSignalChatWidProfileImageClicked(const QString id);

	//需要底部栏闪烁槽函数
	void onSignalTrayIconTwinkle(const bool bNeed);
	//闪烁定时器到时
	void onSignalIconTwinkleTimerout();

	/**
	 * brief：修改侧边栏中头像.
	 * 
	 * \param strImage：传入的头像路径
	 */
	void setProfileImage(const QString& strImage);
	/**
	 * brief:要求打开头像预览、更改界面.
	 * 
	 * \param id：传入的id，根据id显示对应的头像，如果id是自己的则有修改按钮
	 */
	void onSignalChatWidOpenProfileImagePreview(const int id);
	/**
	 * brief：修改了自己头像路径后要更新聊天界面和左上角头像界面.
	 * 
	 * \param imagePath：修改后的头像路径
	 */
	void onSignalProfileImageChanged(const QString& id, const QString& imagePath);

	/**
	 * brief:好友的头像修改了以后就更新一下lastchat，Friendlist，并查看是不是和他聊天呢，是就把chatwidget也更新一下.
	 * 
	 * \param id：好友id
	 * \param imagePath：好友新头像路径
	 */
	void onSignalFriendProfileImageChanged(const QString& id, const QString& imagePath)const;
	
	/**
	 * brief：服务器推送了一个添加你好友的人的头像，更新到model和数据库中.
	 * 
	 * \param id：id
	 * \param imagePath：头像路径
	 */
	void onSignalAddFriendProfileImage(const QString& id, const QString& imagePath)const;

private:
	/**
	 * brief：初始化界面相关的属性.
	 * 
	 */
	void initUi();
	/**
	 * brief:初始化信号槽连接.
	 * 
	 */
	void initConnect();
	//************************************
	// Method:    initData
	// FullName:  ChatWidget::initData
	// Access:    private 
	// Returns:   void
	// Qualifier: 进入聊天界面要进行初始化，获取一些数据
	//************************************
	void initData();
	/**
	 * brief:从备份数据库中获取上次聊天列表，因为修改顺序后是子线程保存在备份数据库中.
	 * 
	 * \param tmpOrder:获取到的上此聊天数据
	 * \param db 要连接的数据库
	 */
	void getLastChatFromBackup(std::vector<QString>& tmpOrder,QSqlDatabase& db);
	/**
	 * brief：和备份数据库进行连接，便于之后的子线程操作.
	 * 
	 * \param db 备份数据库
	 */
	void connectToBackupDB(QSqlDatabase& db);
	//用完了就把临时数据库断开连接
	void disConnectBackupDB(QSqlDatabase& db)const;

private:
	Ui::ChatWidget *ui;
	QObject* m_ptrLastChatQMLRoot{ nullptr }; //上次聊天qml的根对象
	QObject* m_ptrFriendListQMLRoot{ nullptr };//好友列表qml的根对象
	QObject* m_ptrAddFriendQMLRoot{ nullptr };  //添加好友qml界面的根对象
	QTimer* m_ptrNullMessageTimer{ nullptr };   //空的聊天界面
	QQuickWidget* m_ptrLastChatWidget{ nullptr }; //上次聊天列表界面
	QQuickWidget* m_ptrFriendListWidget{ nullptr }; //好友列表界面
	//QQuickWidget* m_ptrNewFriendAndAreadyAddWidget{ nullptr };  //显示已添加和新好友请求的界面
	QQuickWidget* m_ptrSearchFriendList{ nullptr };//搜索好友时显示的界面
	QQuickWidget* m_ptrEmptyWid{ nullptr };     //空的界面
	QSystemTrayIcon* m_ptrTrayIcon{ nullptr };  //托盘图标
	QTimer* m_ptrIconTwinkleTimer{ nullptr };   //托盘图标闪烁用定时器
	TrayIconState m_iTrayState{ TrayIconState::Normal };        //托盘图标的状态，是否为闪烁
	ProfileImagePreview* m_ptrProfileImagePreviewWid{ nullptr };     //头像预览窗口
	MyChatMessageQuickWid* m_ptrChatMessageWid{ nullptr }; //实际的聊天界面
	AddFriendWidget* m_ptrAddFriendWid{ nullptr }; //添加好友界面
	MyFriendListSortModel* m_ptrFriendListModel{ nullptr }; //好友列表model

	//这个用户的id
	QString m_strUserId{""};
	QString m_strUserName{ "" };
	bool m_bLastChatInitFinished{ false };
	std::mutex m_mutex;
	std::condition_variable m_con;
};
