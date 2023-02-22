#include "getProfileImageJsonData.h"

namespace protocol
{
    getProfileImageJsonData::getProfileImageJsonData(const std::string& message/*=""*/)
    {
        parse(message);
    }

    void getProfileImageJsonData::parse(const std::string& message)
    {
        if (message.empty())
        {
            return;
        }
        ptree m_ptree;
        std::stringstream sstream(message);
        read_json(sstream, m_ptree);
        std::string userId = m_ptree.get<std::string>("UserId");
        m_strId = userId;
        return;
    }

    std::string getProfileImageJsonData::generateJson()
    {
        StringBuffer strbuf;
        Writer<rapidjson::StringBuffer> writer(strbuf);

        writer.StartObject();

        writer.Key("type");
        writer.Int(static_cast<int>(m_strType));

        writer.Key("UserId");
        writer.String(m_strId.c_str());

        writer.EndObject();

        return strbuf.GetString();
    }
}

