#include "HeartPackageJsonData.h"

namespace protocol
{
    HeartPackageJsonData::HeartPackageJsonData(const std::string& message /*= ""*/)
    {
        parse(message);
    }

    void HeartPackageJsonData::parse(const std::string& message)
    {
        if (message.empty())
        {
            return;
        }
        ptree m_ptree;
        std::stringstream sstream(message);
        read_json(sstream, m_ptree);
    }

    std::string HeartPackageJsonData::generateJson()
    {
        StringBuffer strbuf;
        Writer<rapidjson::StringBuffer> writer(strbuf);

        writer.StartObject();

        writer.Key("type");
        writer.Int(static_cast<int>(m_strType));

        writer.EndObject();

        return strbuf.GetString();
    }
}

