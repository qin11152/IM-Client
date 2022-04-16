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
}
