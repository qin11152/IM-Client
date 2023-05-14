#include "LoginInReplyData.h"

namespace protocol
{
    LoginInReplyData::LoginInReplyData(const std::string& message /*= ""*/)
    {
        parse(message);
    }

    void LoginInReplyData::parse(const std::string& message)
    {
        if (message.empty())
        {
            return;
        }
        ptree m_ptree;
        std::stringstream sstream(message);
        read_json(sstream, m_ptree);
        bool loginInResult = m_ptree.get<bool>("LoginInResult");
        m_strUserName = m_ptree.get<std::string>("Name");
        m_bLoginInResult = loginInResult;
        return;
    }

    std::string LoginInReplyData::generateJson()
    {
        StringBuffer strbuf;
        Writer<rapidjson::StringBuffer> writer(strbuf);

        writer.StartObject();

        writer.Key("type");
        writer.Int(static_cast<int>(m_strType));

        writer.Key("LoginInResult");
        writer.Bool(m_bLoginInResult);

        writer.Key("Name");
        writer.String(m_strUserName.c_str());

        writer.EndObject();

        return strbuf.GetString();
    }
}
