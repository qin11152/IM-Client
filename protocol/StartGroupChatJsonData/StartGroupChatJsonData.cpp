#include "StartGroupChatJsonData.h"

namespace protocol
{

	StartGroupJsonData::StartGroupJsonData(const std::string& message)
	{
		parse(message);
	}

	StartGroupJsonData::~StartGroupJsonData()
	{
	}

	void StartGroupJsonData::parse(const std::string& message)
	{
		if (message.empty())
		{
			return;
		}
		ptree m_ptree;
		std::stringstream sstream(message);
		read_json(sstream, m_ptree);
		m_strGroupName = m_ptree.get<int>("GroupName");
		m_strImagePathInServer = m_ptree.get<std::string>("ImagePath");
		m_strStarterId = m_ptree.get<std::string>("StartId");
		//获取json对象中id数组
		for (auto& item : m_ptree.get_child("GroupChatId"))
		{
			m_vecGroupChat.push_back(item.second.get_value<std::string>());
		}
	}

	std::string StartGroupJsonData::generateJson()
	{
		//rapisjson生成json
		StringBuffer strbuf;
		Writer<rapidjson::StringBuffer> writer(strbuf);

		writer.StartObject();
		writer.Key("type");
		writer.Int(static_cast<int>(m_strType));

		writer.Key("GroupName");
		writer.String(m_strGroupName.c_str());

		writer.Key("ImagePath");
		writer.String(m_strImagePathInServer.c_str());

		writer.Key("StartId");
		writer.String(m_strStarterId.c_str());

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
