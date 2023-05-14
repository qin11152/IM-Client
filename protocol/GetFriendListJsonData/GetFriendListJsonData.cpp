#include "GetFriendListJsonData.h"

namespace protocol
{
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

        writer.Key("UserId");
        writer.String(m_strUserId.c_str());

        writer.EndObject();

        return strbuf.GetString();
    }
}

