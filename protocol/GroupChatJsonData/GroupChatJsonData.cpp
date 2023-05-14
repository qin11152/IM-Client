#include "GroupChatJsonData.h"

namespace protocol
{
	GroupChatJsonData::GroupChatJsonData(const std::string& message)
	{
		parse(message);
	}

	void GroupChatJsonData::parse(const std::string& message)
	{
		if (message.empty())
		{
			return;
		}
		ptree m_ptree;
		std::stringstream sstream(message);
		read_json(sstream, m_ptree);
		m_strSendId = m_ptree.get<std::string>("SendId");
		m_strMsg = m_ptree.get<std::string>("Msg");
	}

	std::string GroupChatJsonData::generateJson()
	{
		StringBuffer strbuf;
		Writer<rapidjson::StringBuffer> writer(strbuf);

		writer.StartObject();

		writer.Key("type");
		writer.Int(static_cast<int>(m_strType));

		writer.Key("SendId");
		writer.String(m_strSendId.c_str());

		writer.Key("Msg");
		writer.String(m_strMsg.c_str());

		writer.EndObject();
		return strbuf.GetString();
	}
}

