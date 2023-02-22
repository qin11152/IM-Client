#include "LoginInJsonData.h"

namespace protocol
{
    LoginInJsonData::LoginInJsonData(const std::string& message/*=""*/)
    {
        parse(message);
    }

    void LoginInJsonData::parse(const std::string& message)
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
        std::string userpassword = m_ptree.get<std::string>("UserPassword");
        m_strPassword = userpassword;
        return;
    }

    std::string LoginInJsonData::generateJson()
    {
        StringBuffer strbuf;
        Writer<rapidjson::StringBuffer> writer(strbuf);

        writer.StartObject();

        writer.Key("type");
        writer.Int(static_cast<int>(m_strType));

        writer.Key("UserId");
        writer.String(m_strId.c_str());

        writer.Key("UserPassword");
        writer.String(m_strPassword.c_str());

        writer.EndObject();

        return strbuf.GetString();
    }
}

