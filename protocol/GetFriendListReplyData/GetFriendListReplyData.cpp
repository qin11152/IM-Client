#include "GetFriendListReplyData.h"

GetFriendListReplyData::GetFriendListReplyData(const std::string& message)
{
    parse(message);
}

void GetFriendListReplyData::parse(const std::string& message)
{
    if (message.empty())
    {
        return;
    }
    ptree m_ptree;
    ptree m_friendIdList;
    ptree m_friendNamelist;
    std::stringstream sstream(message);
    read_json(sstream, m_ptree);
    //json�����������飬һ��������id��һ��������name������˳���Ӧ����֤����ͬһ��friend
    m_friendIdList = m_ptree.get_child("friendIdList");
    m_friendNamelist = m_ptree.get_child("friendNameList");
    for (auto iter = m_friendIdList.begin(); iter != m_friendIdList.end(); ++iter)
    {
        //��������õ����󣬻�ȡ�����id
        auto item = iter->second;
        //����friendinfo����
        FriendInfo tmp;
        tmp.m_strFriendId = item.get<std::string>("");
        //�洢��vec��
        m_vecFriendList.push_back(tmp);
    }
    int i = 0;//iΪ��ǰ��������Ӧ��vec�е�λ��
    for (auto iter = m_friendNamelist.begin(); iter != m_friendNamelist.end(); ++iter, ++i)
    {
        //�Ѷ�Ӧ������Ҳ������
        auto item = iter->second;
        m_vecFriendList[i].m_strFriendName = item.get<std::string>("");
    }

    return;
}

//���ﲻ��Ҫ�������Ͳ�ʵ���ˣ�ֻ���jsonstringҪ���������ͺã���Ӧ�ķ����ֻ��Ҫ�����ͺ�
std::string GetFriendListReplyData::generateJson()
{
    return "";
}

