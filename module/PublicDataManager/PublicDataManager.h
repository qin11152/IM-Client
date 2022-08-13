//所有公共的数据（如id等）都存储在这，需要的时候从这里调用，单例

#pragma once
#include "module/MyDefine.h"
#include <boost/serialization/singleton.hpp>
#include <QObject>
using namespace boost;

class PublicDataManager:public serialization::singleton<PublicDataManager>
{
public:

    std::vector<MyFriendInfoWithFirstC>& getMyFriendInfoWithCVec();
    std::vector<MyLastChatFriendInfo>& getMyLastChatFriendInfoVec();
    std::unordered_map<QString, int>& getMyUsetInfoMap();
    QString getMyId()const;
    QString getMyName()const;

    void setMyFriendInfoWithC(const std::vector<MyFriendInfoWithFirstC>& vecFriendInfoWithC);
    void setMyLastChatInfo(const std::vector<MyLastChatFriendInfo>& vecLastChatFriend);
    void setUserInfo(const std::unordered_map<QString, int>& mapUserInfo);
    void setMyId(const QString& id);
    void setMyName(const QString& name);
private:
    QString m_strId{ "" };
    QString m_strName{ "" };

    //存储好友信息，带首字母，这个是从服务器得到的，有具体信息
    std::vector<MyFriendInfoWithFirstC> m_vecFriendInfoWithC;
    std::unordered_map<QString, int> m_mapUserInfo;     //存储好友id和该id在vec中对应的位置，以便查找信息
    std::vector<MyLastChatFriendInfo> m_vecLastChatFriend;        //上次聊天页面里的好友,这个是从数据库得到的顺序，只有id不包含其他信息
};

