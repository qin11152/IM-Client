#include "SingleChatMessageJsonData.h"

SingleChatMessageJsonData::SingleChatMessageJsonData(const std::string& message /*= ""*/)
{
    parse(message);
}

void SingleChatMessageJsonData::parse(const std::string& message)
{
    if (message.empty())
    {
        return;
    }
    ptree m_ptree;
    std::stringstream sstream;
    read_json(sstream, m_ptree);
    m_strSendUserId = m_ptree.get<std::string>("SendUserId");
    m_strRecvUserId = m_ptree.get<std::string>("RecvUserId");
    m_strMessage = m_ptree.get<std::string>("Message");
    m_strTime = m_ptree.get<std::string>("Time");
}

std::string SingleChatMessageJsonData::generateJson()
{
    StringBuffer strbuf;
    Writer<StringBuffer> writer(strbuf);
    
    writer.StartObject();

    writer.Key("type");
    writer.Int(static_cast<int>(m_strType));

    writer.Key("SendUserId");
    writer.String(m_strSendUserId.c_str());

    writer.Key("RecvUserId");
    writer.String(m_strRecvUserId.c_str());

    writer.Key("Message");
    writer.String(m_strMessage.c_str());

    writer.Key("Time");
    writer.String(m_strTime.c_str());

    writer.EndObject();
    return strbuf.GetString();
}
