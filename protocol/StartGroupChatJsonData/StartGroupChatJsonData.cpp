#include "StartGroupChatJsonData.h"

namespace protocol
{
	StartGroupChatJsonData::StartGroupChatJsonData(const std::string& message)
	{
		parse(message);
	}

	void StartGroupChatJsonData::parse(const std::string& message)
	{
		if (message.empty())
		{
			return;
		}
		ptree m_ptree;
		std::stringstream sstream(message);
		read_json(sstream, m_ptree);
		m_strStarterId = m_ptree.get<std::string>("StarterId");
		m_strGuid = m_ptree.get<std::string>("Guid");
		//从json数组中提取gruopchatid
		for (auto& item : m_ptree.get_child("GroupChatId"))
		{
			m_vecGroupChat.push_back(item.second.get_value<std::string>());
		}
		return;
	}

	std::string StartGroupChatJsonData::generateJson()
	{
		StringBuffer strbuf;
		Writer<rapidjson::StringBuffer> writer(strbuf);

		writer.StartObject();

		writer.Key("type");
		writer.Int(static_cast<int>(m_strType));

		writer.Key("StarterId");
		writer.String(m_strStarterId.c_str());

		writer.Key("Guid");
		writer.String(m_strGuid.c_str());

		writer.Key("GroupChatId");
		writer.StartArray();
		for (auto& item : m_vecGroupChat)
		{
			writer.String(item.c_str());
		}
		writer.EndArray();

		writer.EndObject();
		return strbuf.GetString();
	}
}
