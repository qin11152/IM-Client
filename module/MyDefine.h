#pragma once
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#define coid void;
#include <QObject>
#include <string>

constexpr int PackageLength = 8;
constexpr int TwinkleTime = 700;
inline const char* kDefaultProfileImage = "qrc:///LogInWidget/image/lv.jpg";
inline const char* kDefaultProfileImageWidget = ":LogInWidget/image/lv.jpg";
constexpr int kSegmentLength = 9000;

constexpr int kHeartTimeoutTime = 300 * 1000;
constexpr int kKeepAlivePackageTime = 299 * 1000;
constexpr int kMsgBufferLength = 1024 * 10;

constexpr char kHostIp[] = "43.142.158.231";
constexpr int kHostPort = 9999;

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
	GetFriendList,
	GetFriendListReply,
	ProfileImageMsg,
	getFriendProfileImage,
	getFriendProfileImageReply,
};

/**
 * brief�����ݲ�ͬ������������ͷ��������ͬ����.
 */
enum class ProfileImageType
{
	UpdateProfileImage=0,       //Ҫ�����ͷ��
	AddFriendProfileImage,      //��Ӻ���ʱ���͵�ͷ��
};

enum class UserRoleDefine
{
	//��Ӻ���ģ����ʹ��
	AddFriendName = Qt::UserRole + 1,
	AddFriendImagePath = Qt::UserRole + 2,
	AddFriednVerifyInfo = Qt::UserRole + 3,
	AddFriendValid = Qt::UserRole + 4,
	AddFriendId = Qt::UserRole + 5,

	//�����б���ʹ��
	FriendListId = Qt::UserRole + 6,
	FriendListName = Qt::UserRole + 7,
	FriendListImagePath = Qt::UserRole + 8,
};

enum FriendListType
{
	LastChatWidget,
	FriendListWidget,
	SearchFriendWidget
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
	//ͷ���ʱ����������ͬ˵���޸Ĺ��ˣ������»�ȡһ��ͷ��
	std::string m_strImageTimestamp{ "" };

	/*MyFriendInfoWithFirstC(std::string name, std::string imagePath, std::string id, std::string firstC, )
	{
		m_strName = name;
		m_strImagePath = imagePath;
		m_strId = id;
		m_strFirstChacter = firstC;
	}*/
};

class FriendInfo
{
public:
	std::string m_strFriendId{ "" };      //���ѵ�id
	std::string m_strFriendName{ "" };     //���ѵ��ǳ�
	std::string m_strFriendImageTimeStamp{ "" };    //����ͷ��ʱ���
};

using MessageTypeBaseType = std::underlying_type<MessageType>::type;

struct MyLastChatFriendInfo
{
public:
	QString m_strId;
	QString m_strName;
	MyLastChatFriendInfo(QString name="", QString id="") :
		m_strId(id),
		m_strName(name)
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
	QString m_strId{ "" };              //����id
	QString m_strFriendName{ "" };      //���ѵ�����
	QString m_strProfileImagePath{ "" };//����ͷ���·��
	QString m_strVerifyInfo{ "" };      //���ѵ���֤��Ϣ
	bool isValid{ false };              //�Ƿ��Ѿ����˺���
};

/**
 * brief ��ǰ��������е����������Ϣ
 */
struct CurrentChatWidgetUserInfo
{
	//��ǰ�����е������¼����
	int recordCount;
	//��ǰ��������������˵�id
	std::string userId;

	CurrentChatWidgetUserInfo(int cnt = 10, std::string id = "0")
		:recordCount(cnt),
		userId(id)
	{
	}
};
/************************************************************************/
/* ��������                                                                     */
/************************************************************************/


