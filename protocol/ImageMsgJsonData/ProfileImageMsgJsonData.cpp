#include "ProfileImageMsgJsonData.h"

ProfileImageMsgJsonData::ProfileImageMsgJsonData(const std::string& message/*=""*/)
{
    parse(message);
}

void ProfileImageMsgJsonData::parse(const std::string& message)
{
    if (message.empty())
    {
        return;
    }
    ptree m_ptree;
    std::stringstream sstream(message);
    read_json(sstream, m_ptree);
    m_strId = m_ptree.get<std::string>("UserId");
    m_strUUID = m_ptree.get<std::string>("ImageUUID");
    m_strBase64Msg = m_ptree.get<std::string>("ImageBase64Msg");
    m_strImageName = m_ptree.get<std::string>("ImageName");
    m_strSuffix = m_ptree.get<std::string>("Suffix");
    m_iCurIndex = m_ptree.get<int>("ImageIndex");
    m_iSumIndex = m_ptree.get<int>("SumIndex");
}

std::string ProfileImageMsgJsonData::generateJson()
{

    StringBuffer strbuf;
    Writer<rapidjson::StringBuffer> writer(strbuf);

    writer.StartObject();

    writer.Key("type");
    writer.Int(static_cast<int>(m_strType));

    writer.Key("UserId");
    writer.String(m_strId.c_str());

    writer.Key("ImageUUID");
    writer.String(m_strUUID.c_str());

    writer.Key("ImageBase64Msg");
    writer.String(m_strBase64Msg.c_str());

    writer.Key("ImageName");
    writer.String(m_strImageName.c_str());

    writer.Key("Suffix");
    writer.String(m_strSuffix.c_str());

    writer.Key("ImageIndex");
    writer.Int(m_iCurIndex);

    writer.Key("SumIndex");
    writer.Int(m_iSumIndex);

    writer.EndObject();

    return strbuf.GetString();
}
