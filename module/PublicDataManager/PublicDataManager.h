//所有公共的数据（如id等）都存储在这，需要的时候从这里调用，单例

#pragma once
#include "module/stdafx.h"

#include <boost/serialization/singleton.hpp>
#include <QObject>
using namespace boost;

class PublicDataManager:public serialization::singleton<PublicDataManager>
{
public:

    std::vector<MyFriendInfoWithFirstC>& getMyFriendInfoWithCVec();
    std::vector<MyLastChatFriendInfo>& getMyLastChatFriendInfoVec();
    std::vector<MyFriendInfoForStartGroupChat>& getFriendInfoForStartGroupChat();
    void getFriendImagePathVec(std::vector<std::string>& vecId);
    std::unordered_map<QString, int>& getMyUsetInfoMap();
    QString getMyId()const;
    QString getMyName()const;
    CurrentChatWidgetUserInfo getCurrentChatWidgetUserInfo()const;

    //根据id获取用户信息
    MyFriendInfoWithFirstC getFriendInfoAcordId(const QString& id);

    void setMyFriendInfoWithC(const std::vector<MyFriendInfoWithFirstC>& vecFriendInfoWithC);
    void setMyLastChatInfo(const std::vector<MyLastChatFriendInfo>& vecLastChatFriend);
    void setUserInfo(const std::unordered_map<QString, int>& mapUserInfo);
    void setMyId(const QString& id);
    void setMyName(const QString& name);
    void setCurrentChatWidgetUserInfo(const CurrentChatWidgetUserInfo& info);
    void setUnreadMsg(const QString& id, int cnt);
    void setIdDirPath(const QString& path);
    void setImagePath(const QString& path);
    /**
     * brief：好友头像修改后要将其在vec中的头像路径修改一下.
     * 
     * \param id：好友id
     * \param path：好友头像的路径
     */
    void updateProfileImagePathOfFriendInfoVec(const QString& id, const QString& path);
    void clearUnreadMsg(const QString& id);
    int getUnreadMsgCnt(const QString& id)const;
    QString getIdDirPath()const;
    QString getImagePath()const;

    /**
     * brief：当添加了好友以后添加到vec中.
     * 
     * \param friendInfo：新添加的好友的信息
     */
    void addFriendInfoWithC(const MyFriendInfoWithFirstC& friendInfo);

    bool isIdExistInLastChatList(const QString& id)const;
    void insertLastChatList(const MyLastChatFriendInfo& info);
private:
    QString m_strId{ "" };
    QString m_strName{ "" };
    QString m_strImagePath{ "" };
    //自己头像的时间戳
    QString m_strMyImageTimeStamp{ "" };
    QString m_strIdDirPath{ "" };   //自己id所在的文件夹路径

    std::unordered_map<QString, int> m_mapUnreadMsgCnt;     //记录不同id对应的未读消息
    std::vector<MyFriendInfoWithFirstC> m_vecFriendInfoWithC;    //存储好友信息，带首字母，这个是从服务器得到的，有具体信息
    std::vector<MyFriendInfoForStartGroupChat> m_vecFriendInfoForStartGroupChat;    //好友信息，用于发起群聊
    std::unordered_map<QString, int> m_mapUserInfo;     //存储好友id和该id在vec中对应的位置，以便查找信息
    std::vector<MyLastChatFriendInfo> m_vecLastChatFriend;        //上次聊天页面里的好友,这个是从数据库得到的顺序，只有id不包含其他信息
    CurrentChatWidgetUserInfo m_stuCurrentChatUserInfo;     //当前聊天界面中的相关信息（聊天记录数量，用户id）
};

