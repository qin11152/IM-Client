#include "StartGroupChatReplyJsonData.h"

namespace protocol
{
	StartGroupChatReplyJsonData::StartGroupChatReplyJsonData(const std::string& message)
	{
		parse(message);
	}

	void StartGroupChatReplyJsonData::parse(const std::string& message)
	{
		if (message.empty())
		{
			return;
		}
		ptree m_ptree;
		std::stringstream sstream(message);
		read_json(sstream, m_ptree);
		m_strGroupChatId = m_ptree.get<std::string>("GroupChatId");
		m_strGuid = m_ptree.get<std::string>("Guid");
		m_bIsSuccess = m_ptree.get<bool>("IsSuccess");
	}

	std::string StartGroupChatReplyJsonData::generateJson()
	{
		StringBuffer strbuf;
		Writer<rapidjson::StringBuffer> writer(strbuf);

		writer.StartObject();

		writer.Key("type");
		writer.Int(static_cast<int>(m_strType));

		writer.Key("GroupChatId");
		writer.String(m_strGroupChatId.c_str());

		writer.Key("Guid");
		writer.String(m_strGuid.c_str());

		writer.Key("IsSuccess");
		writer.Bool(m_bIsSuccess);

		writer.EndObject();
		return strbuf.GetString();
	}
}

