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
		m_strGroupId = m_ptree.get<std::string>("GroupId");
		m_strGuid = m_ptree.get<std::string>("Guid");
		m_bIsSuccess = m_ptree.get<bool>("IsSuccess");
		for (auto& item : m_ptree.get_child("GroupChatVecId"))
		{
			m_vecGroupChatId.push_back(item.second.get_value<std::string>());
		}
	}

	std::string StartGroupChatReplyJsonData::generateJson()
	{
		StringBuffer strbuf;
		Writer<rapidjson::StringBuffer> writer(strbuf);

		writer.StartObject();

		writer.Key("type");
		writer.Int(static_cast<int>(m_strType));

		writer.Key("GroupId");
		writer.String(m_strGroupId.c_str());

		writer.Key("Guid");
		writer.String(m_strGuid.c_str());

		writer.Key("GroupChatId");
		writer.StartArray();
		for (auto& id : m_vecGroupChatId)
		{
			writer.String(id.c_str());
		}
		writer.EndArray();
		

		writer.Key("IsSuccess");
		writer.Bool(m_bIsSuccess);

		writer.EndObject();
		return strbuf.GetString();
	}
}

