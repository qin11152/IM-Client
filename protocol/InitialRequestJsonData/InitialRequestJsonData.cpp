#include "InitialRequestJsonData.h"

namespace protocol
{
    InitialRequestJsonData::InitialRequestJsonData(const std::string& message /*= ""*/)
    {
        parse(message);
    }

    void InitialRequestJsonData::parse(const std::string& message)
    {
        if (message.empty())
        {
            return;
        }
        ptree m_ptree;
        std::stringstream sstream(message);
        read_json(sstream, m_ptree);
        m_strId = m_ptree.get<std::string>("UserId");
        return;
    }

    std::string InitialRequestJsonData::generateJson()
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

