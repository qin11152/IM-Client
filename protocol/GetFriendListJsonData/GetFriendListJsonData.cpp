#include "GetFriendListJsonData.h"

GetFriendListJsonData::GetFriendListJsonData(const std::string& message /*= ""*/)
{
    parse(message);
}

void GetFriendListJsonData::parse(const std::string& message)
{
}

std::string GetFriendListJsonData::generateJson()
{
    StringBuffer strbuf;
    Writer<rapidjson::StringBuffer> writer(strbuf);

    writer.StartObject();

    writer.Key("type");
    writer.Int(static_cast<int>(m_strType));

    writer.EndObject();

    return strbuf.GetString();
}
