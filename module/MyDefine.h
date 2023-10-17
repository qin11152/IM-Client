﻿#pragma once
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#define coid void;
#include <QObject>
#include <string>

#define Test

constexpr int TwinkleTime = 700;
inline const char* kDefaultProfileImage = "qrc:///LogInWidget/image/lv.jpg";
inline const char* kDefaultProfileImageWidget = ":LogInWidget/image/lv.jpg";
constexpr int kSegmentLength = 9000;

constexpr int kHeartTimeoutTime = 300 * 1000;
constexpr int kKeepAlivePackageTime = 299 * 1000;
constexpr size_t kMsgBufferLength = 1024 * 10;

constexpr char kHostIp[] = "43.142.158.231";
constexpr int kHostPort = 19999;

constexpr int kMaxGroupName = 15;

constexpr char kProfileTableName[] = "profileImage";
constexpr char kLastChatTableName[] = "lastChatList";
constexpr char kFriendRequetName[] = "friendRequest";

Q_DECLARE_METATYPE(std::string);

enum class DatabaseOperateType
{
	None = 0,
	UpdateLastChat,
};

enum class MessageType
{
	Default,
	RegisterRequest,
	RegisterResponse,
	LoginRequest,
	LoginResponse,
	InitialRequest,
	StartGroupChat,
	StartGroupChatReply,
	GroupChat,
	SingleChat,
	AddFriendRequest,
	AddFriendResponse,
	AddFriendNotify,
	HeartPackage,
	FriendListRequest,
	FriendListResponse,
	RefreshFriendListRequest,
	RefreshFriendListResponse,
	GetFriendListRequest,
	GetFriendListReply,
	ProfileImageMsg,
	getFriendProfileImage,
	getFriendProfileImageReply,
};

enum class ImageMessageType
{
	Normal = 0,
};

/**
 * brief：根据不同的需求来传递头像，做出不同处理.
 */
enum class ProfileImageType
{
	UpdateProfileImage=0,       //要求更新头像
	AddFriendProfileImage,      //添加好友时传送的头像
};

enum class UserRoleDefine
{
	//添加好友模型中使用
	AddFriendName = Qt::UserRole + 1,
	AddFriendImagePath = Qt::UserRole + 2,
	AddFriednVerifyInfo = Qt::UserRole + 3,
	AddFriendValid = Qt::UserRole + 4,
	AddFriendId = Qt::UserRole + 5,

	//好友列表中使用
	FriendListId = Qt::UserRole + 6,
	FriendListName = Qt::UserRole + 7,
	FriendListImagePath = Qt::UserRole + 8,
	
	//选择好友发起群聊列表中是否为选中状态
	ChooseFriendIsSelected = Qt::UserRole + 9,
};

enum FriendListType
{
	LastChatWidget,
	FriendListWidget,
	SearchFriendWidget
};

//当前聊天窗口的状态，空，单聊或群聊
enum class CurrentChatWidgetState
{
	Empty=0,
	Single,
	Group
};

enum class TrayIconState
{
	Normal=0,
	None
};

struct MyFriendInfo
{
public:
	std::string m_strName;
	std::string m_strImagePath;
	int m_iId;
};

struct MyFriendInfoWithFirstC
{
public:
	std::string m_strName{""};
	std::string m_strImagePath{ "qrc:///LogInWidget/image/lv.jpg" };
	std::string m_strId{""};
	std::string m_strFirstChacter{""};
	//头像的时间戳，如果不同说明修改过了，就重新获取一下头像
	std::string m_strImageTimestamp{ "" };

	bool m_bIsGroupChat{ false };

	/*MyFriendInfoWithFirstC(std::string name, std::string imagePath, std::string id, std::string firstC, )
	{
		m_strName = name;
		m_strImagePath = imagePath;
		m_strId = id;
		m_strFirstChacter = firstC;
	}*/
};

struct MyFriendInfoForStartGroupChat
{
public:
	std::string m_strName{ "" };
	std::string m_strImagePath{ "qrc:///LogInWidget/image/lv.jpg" };
	std::string m_strId{ "" };

	bool m_bIsSelected{ false };
};

class FriendInfo
{
public:
	std::string m_strFriendId{ "" };      //好友的id
	std::string m_strFriendName{ "" };     //好友的昵称
	std::string m_strFriendImageTimeStamp{ "" };    //好友头像时间戳
};

using MessageTypeBaseType = std::underlying_type<MessageType>::type;

struct ChatBaseInfo
{
};

struct MyLastChatFriendInfo
{
public:
	QString m_strId;
	QString m_strName;
	bool m_bIsGroup{ false };
	MyLastChatFriendInfo(QString name = "", QString id = "", bool bIsGroupChat = false) :
		m_strId(id),
		m_strName(name),
		m_bIsGroup(bIsGroupChat)
	{}
};

struct MyLastChatGroupInfo
{
public:
	QString m_strGroupId;
	QString m_strGroupName;
	bool m_bIsGroup{ true };
	MyLastChatGroupInfo(QString name = "", QString id = "", bool bIsGroupChat = false) :
		m_strGroupId(id),
		m_strGroupName(name),
		m_bIsGroup(bIsGroupChat)
	{}
};

struct MyChatMessageInfo
{
public:
	QString m_strMessage;
	QString m_strName;
	QString m_strTime;
	bool m_bIsSelf;
	MyChatMessageInfo(QString message="", QString name="", QString time="", bool isSelf="") :
		m_strMessage(message),
		m_strName(name),
		m_strTime(time),
		m_bIsSelf(isSelf)
	{}
};

struct MyAddFriendInfo
{
public:
	QString m_strFriendName;
	QString m_strFriendId;
	QString m_strVerifyMsg;
	bool m_bIsValid{ false };
	MyAddFriendInfo(QString name = "", QString id = "", QString verifyMsg = "", bool isValid = false)
		:m_strFriendName(name),
		m_strFriendId(id),
		m_strVerifyMsg(verifyMsg),
		m_bIsValid(isValid)
	{}
};

struct AddFriendInfo
{
	QString m_strId{ "" };              //好友id
	QString m_strFriendName{ "" };      //好友的名字
	QString m_strProfileImagePath{ "" };//好友头像的路径
	QString m_strVerifyInfo{ "" };      //好友的验证消息
	bool isValid{ false };              //是否已经加了好友
};

/**
 * brief 当前聊天界面中的聊天相关信息
 */
struct CurrentChatWidgetUserInfo
{
	//当前界面中的聊天记录数量
	int recordCount;
	//当前聊天界面中聊天人的id
	std::string userId;

	CurrentChatWidgetUserInfo(int cnt = 10, std::string id = "0")
		:recordCount(cnt),
		userId(id)
	{
	}
};

struct MyGroupChatInfo
{
	//当前界面中聊天记录的数量
	int recordCount;
	//当前聊天界面中聊天群的id
	std::string groupId;

	MyGroupChatInfo(int cnt = 10, std::string id = "0")
		:recordCount(cnt),
		groupId(id)
	{
	}
};

Q_DECLARE_METATYPE(MyGroupChatInfo);
Q_DECLARE_METATYPE(CurrentChatWidgetUserInfo);
Q_DECLARE_METATYPE(AddFriendInfo);
Q_DECLARE_METATYPE(MyAddFriendInfo)
Q_DECLARE_METATYPE(MyChatMessageInfo)
Q_DECLARE_METATYPE(MyLastChatGroupInfo)
Q_DECLARE_METATYPE(MyLastChatFriendInfo)
Q_DECLARE_METATYPE(FriendInfo)
Q_DECLARE_METATYPE(MyFriendInfoForStartGroupChat)
Q_DECLARE_METATYPE(MyFriendInfoWithFirstC)
Q_DECLARE_METATYPE(MyFriendInfo)

/************************************************************************/
/* 公共函数                                                                     */
/************************************************************************/




