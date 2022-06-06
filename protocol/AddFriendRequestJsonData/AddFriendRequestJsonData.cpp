#include "AddFriendRequestJsonData.h"

AddFriendRequestJsonData::AddFriendRequestJsonData(const std::string& message/*=""*/)
{
    parse(message);
}

void AddFriendRequestJsonData::parse(const std::string& message/*=""*/)
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
    m_strVerifyMsg = m_ptree.get<bool>("VerifyMsg");
    m_strName = m_ptree.get<std::string>("Name");
    return;
}

std::string AddFriendRequestJsonData::generateJson()
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

    writer.Key("VerifyMsg");
    writer.String(m_strVerifyMsg.c_str());

    writer.Key("Name");
    writer.String(m_strName.c_str());

    writer.EndObject();

    return strbuf.GetString();
}

