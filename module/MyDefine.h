#pragma once

#define coid void;
#include <string>

namespace
{

    constexpr int PackageLength = 8;

    enum FriendListType
    {
        FriendListWidget,
        SearchWidget
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
        std::string m_strFriendId{ nullptr };      //���ѵ�id
        std::string m_strFriendName{ nullptr };     //���ѵ��ǳ�
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
}
