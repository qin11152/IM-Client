#include "RegisterJsonData.h"

namespace protocol
{
    RegisterJsonData::RegisterJsonData(const std::string& message)
    {
        parse(message);
    }

    void RegisterJsonData::parse(const std::string& message)
    {
        if (message.empty())
        {
            return;
        }
        ptree m_ptree;
        std::stringstream sstream(message);
        read_json(sstream, m_ptree);
        std::string userName = m_ptree.get<std::string>("UserName");
        m_strUserName = userName;
        std::string userpassword = m_ptree.get<std::string>("UserPassword");
        m_strUserPassword = userpassword;
        return;
    }

    std::string RegisterJsonData::generateJson()
    {
        StringBuffer strbuf;
        Writer<rapidjson::StringBuffer> writer(strbuf);

        writer.StartObject();

        writer.Key("type");
        writer.Int(static_cast<int>(m_strType));

        writer.Key("UserName");
        writer.String(m_strUserName.c_str());

        writer.Key("UserPassword");
        writer.String(m_strUserPassword.c_str());

        writer.EndObject();

        return strbuf.GetString();
    }
}
