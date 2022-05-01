#pragma once

#define coid void;
#include <QObject>
#include <string>


    constexpr int PackageLength = 8;

    enum FriendListType
    {
        LastChatWidget,
        FriendListWidget
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
        std::string m_strName;
        std::string m_strImagePath;
        int m_iId;
        std::string m_strFirstChacter;
    };

    class FriendInfo
    {
    public:
        std::string m_strFriendId{ "" };      //���ѵ�id
        std::string m_strFriendName{ "" };     //���ѵ��ǳ�
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
        HeartPackage,
        FriendListRequest,
        FriendListResponse,
        GetFriendList,
        GetFriendListReply
    };

    using MessageTypeBaseType = std::underlying_type<MessageType>::type;

    struct MyChatMessageInfo
    {
    public:
        QString m_strMessage;
        QString m_strName;
        QString m_strTime;
        bool m_bIsSelf;
        MyChatMessageInfo(QString message, QString name, QString time, bool isSelf) :
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
        MyAddFriendInfo(QString name, QString id, QString verifyMsg, bool isValid)
            :m_strFriendName(name),
            m_strFriendId(id),
            m_strVerifyMsg(verifyMsg),
            m_bIsValid(isValid)
        {}
    };
