//���й��������ݣ���id�ȣ����洢���⣬��Ҫ��ʱ���������ã�����

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

    //����id��ȡ�û���Ϣ
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
     * brief������ͷ���޸ĺ�Ҫ������vec�е�ͷ��·���޸�һ��.
     * 
     * \param id������id
     * \param path������ͷ���·��
     */
    void updateProfileImagePathOfFriendInfoVec(const QString& id, const QString& path);
    void clearUnreadMsg(const QString& id);
    int getUnreadMsgCnt(const QString& id)const;
    QString getIdDirPath()const;
    QString getImagePath()const;

    /**
     * brief��������˺����Ժ���ӵ�vec��.
     * 
     * \param friendInfo������ӵĺ��ѵ���Ϣ
     */
    void addFriendInfoWithC(const MyFriendInfoWithFirstC& friendInfo);

    bool isIdExistInLastChatList(const QString& id)const;
    void insertLastChatList(const MyLastChatFriendInfo& info);
private:
    QString m_strId{ "" };
    QString m_strName{ "" };
    QString m_strImagePath{ "" };
    //�Լ�ͷ���ʱ���
    QString m_strMyImageTimeStamp{ "" };
    QString m_strIdDirPath{ "" };   //�Լ�id���ڵ��ļ���·��

    std::unordered_map<QString, int> m_mapUnreadMsgCnt;     //��¼��ͬid��Ӧ��δ����Ϣ
    std::vector<MyFriendInfoWithFirstC> m_vecFriendInfoWithC;    //�洢������Ϣ��������ĸ������Ǵӷ������õ��ģ��о�����Ϣ
    std::vector<MyFriendInfoForStartGroupChat> m_vecFriendInfoForStartGroupChat;    //������Ϣ�����ڷ���Ⱥ��
    std::unordered_map<QString, int> m_mapUserInfo;     //�洢����id�͸�id��vec�ж�Ӧ��λ�ã��Ա������Ϣ
    std::vector<MyLastChatFriendInfo> m_vecLastChatFriend;        //�ϴ�����ҳ����ĺ���,����Ǵ����ݿ�õ���˳��ֻ��id������������Ϣ
    CurrentChatWidgetUserInfo m_stuCurrentChatUserInfo;     //��ǰ��������е������Ϣ�������¼�������û�id��
};

