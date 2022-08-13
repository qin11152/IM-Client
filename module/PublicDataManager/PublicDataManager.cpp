#include "PublicDataManager.h"

std::vector<MyFriendInfoWithFirstC>& PublicDataManager::getMyFriendInfoWithCVec()
{
    return m_vecFriendInfoWithC;
}

std::vector<MyLastChatFriendInfo>& PublicDataManager::getMyLastChatFriendInfoVec()
{
    return m_vecLastChatFriend;
}

std::unordered_map<QString, int>& PublicDataManager::getMyUsetInfoMap()
{
    return m_mapUserInfo;
}

QString PublicDataManager::getMyId() const
{
    return m_strId;
}

QString PublicDataManager::getMyName() const
{
    return m_strName;
}

void PublicDataManager::setMyFriendInfoWithC(const std::vector<MyFriendInfoWithFirstC>& vecFriendInfoWithC)
{
    m_vecFriendInfoWithC = vecFriendInfoWithC;
}

void PublicDataManager::setMyLastChatInfo(const std::vector<MyLastChatFriendInfo>& vecLastChatFriend)
{
    m_vecLastChatFriend = vecLastChatFriend;
}

void PublicDataManager::setUserInfo(const std::unordered_map<QString, int>& mapUserInfo)
{
    m_mapUserInfo = mapUserInfo;
}

void PublicDataManager::setMyId(const QString& id)
{
    m_strId = id;
}

void PublicDataManager::setMyName(const QString& name)
{
    m_strName = name;
}
