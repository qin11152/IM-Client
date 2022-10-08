#pragma once

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

Q_DECLARE_METATYPE(std::string);

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

enum class DatabaseOperateType
{
    None=0,
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

/************************************************************************/
/* ��������                                                                     */
/************************************************************************/


