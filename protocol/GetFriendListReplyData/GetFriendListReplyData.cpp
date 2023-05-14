#include "GetFriendListReplyData.h"

namespace protocol
{
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
#if 0
        ptree m_ptree;
        ptree m_friendIdList;
        ptree m_friendNamelist;
        std::stringstream sstream(message);
        read_json(sstream, m_ptree);
        //json中有两个数组，一个数组存放id，一个数组存放name，两者顺序对应，保证属于同一个friend
        m_friendIdList = m_ptree.get_child("FriendIdList");
        m_friendNamelist = m_ptree.get_child("FriendNameList");
        for (auto iter = m_friendIdList.begin(); iter != m_friendIdList.end(); ++iter)
        {
            //遍历数组得到对象，获取对象的id
            auto item = iter->second;
            //构建friendinfo对象
            FriendInfo tmp;
            tmp.m_strFriendId = item.get<std::string>("");
            //存储在vec中
            m_vecFriendList.push_back(tmp);
        }
        int i = 0;//i为当前迭代器对应的vec中的位置
        for (auto iter = m_friendNamelist.begin(); iter != m_friendNamelist.end(); ++iter, ++i)
        {
            //把对应的名字也解析了
            auto item = iter->second;
            m_vecFriendList[i].m_strFriendName = item.get<std::string>("");
        }
#endif
        ptree ptrees;
        ptree listNode;
        std::stringstream sstream(message);
        read_json(sstream, ptrees);
        listNode = ptrees.get_child("friendInfoList");
        for (auto iter = listNode.begin(); iter != listNode.end(); ++iter)
        {
            auto item = iter->second;
            FriendInfo tmp;
            tmp.m_strFriendId = item.get<std::string>("id");
            tmp.m_strFriendName = item.get<std::string>("name");
            tmp.m_strFriendImageTimeStamp = item.get<std::string>("imageTimeStamp");
            m_vecFriendList.push_back(tmp);
        }
    }

    //这里不需要构建，就不实现了，只需把jsonstring要解析出来就好，对应的服务端只需要构建就好
    std::string GetFriendListReplyData::generateJson()
    {
        return "";
    }
}

