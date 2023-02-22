#include "AddFriendResponseJsonData.h"

namespace protocol
{
    AddFriendResponseJsonData::AddFriendResponseJsonData(const std::string& message /*= ""*/)
    {
        parse(message);
    }

    void AddFriendResponseJsonData::parse(const std::string& message)
    {
        if (message.empty())
        {
            return;
        }
        ptree m_ptree;
        std::stringstream sstream(message);
        read_json(sstream, m_ptree);
        m_strMyId = m_ptree.get<std::string>("MyId");
        m_strFriendId = m_ptree.get<std::string>("FriendId");
        m_bResult = m_ptree.get<bool>("AddFriendResult");
        return;
    }

    std::string AddFriendResponseJsonData::generateJson()
    {
        StringBuffer strbuf;
        Writer<rapidjson::StringBuffer> writer(strbuf);

        writer.StartObject();

        writer.Key("type");
        writer.Int(static_cast<int>(m_strType));

        writer.Key("MyId");
        writer.String(m_strMyId.c_str());

        writer.Key("FriendId");
        writer.String(m_strFriendId.c_str());

        writer.Key("AddFriendResult");
        writer.Bool(m_bResult);

        writer.EndObject();

        return strbuf.GetString();
    }
}

