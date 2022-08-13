//���й��������ݣ���id�ȣ����洢���⣬��Ҫ��ʱ���������ã�����

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

    //�洢������Ϣ��������ĸ������Ǵӷ������õ��ģ��о�����Ϣ
    std::vector<MyFriendInfoWithFirstC> m_vecFriendInfoWithC;
    std::unordered_map<QString, int> m_mapUserInfo;     //�洢����id�͸�id��vec�ж�Ӧ��λ�ã��Ա������Ϣ
    std::vector<MyLastChatFriendInfo> m_vecLastChatFriend;        //�ϴ�����ҳ����ĺ���,����Ǵ����ݿ�õ���˳��ֻ��id������������Ϣ
};

