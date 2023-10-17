#include "StartGroupChatJsonData.h"

namespace protocol
{
	StartGroupChatInnerData::StartGroupChatInnerData(const std::string& message)
	{
		parse(message);
	}

	StartGroupChatInnerData::~StartGroupChatInnerData()
	{
	}

	void StartGroupChatInnerData::parse(const std::string& message)
	{
		if (message.empty())
		{
			return;
		}
		ptree m_ptree;
		std::stringstream sstream(message);
		read_json(sstream, m_ptree);
		m_strGroupName = m_ptree.get<std::string>("GroupName");
		m_strGuid = "";
		m_strStarterId = m_ptree.get<std::string>("StartId");
		//从json数组中提取gruopchatid
		for (auto& item : m_ptree.get_child("GroupChatId"))
		{
			m_vecGroupChat.push_back(item.second.get_value<std::string>());
		}
	}

	std::string StartGroupChatInnerData::generateJson()
	{
		//rapisjson生成json
		StringBuffer strbuf;
		Writer<rapidjson::StringBuffer> writer(strbuf);

		writer.StartObject();

		writer.Key("type");
		writer.Int(static_cast<int>(m_strType));

		writer.Key("StarterId");
		writer.String(m_strStarterId.c_str());

		writer.Key("Guid");
		writer.String(m_strGuid.c_str());

		writer.Key("GroupName");
		writer.String(m_strGroupName.c_str());

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
		m_iImageLenth = m_ptree.get<int>("ImageLength");
		m_strImageSuffix = m_ptree.get<std::string>("ImageSuffix");
		m_stInnerData.parse(m_ptree.get<std::string>("InnerData"));
	}

	std::string StartGroupJsonData::generateJson()
	{
		//rapisjson生成json
		StringBuffer strbuf;
		Writer<rapidjson::StringBuffer> writer(strbuf);

		writer.StartObject();
		writer.Key("type");
		writer.Int(static_cast<int>(m_strType));

		writer.Key("ImageLength");
		writer.Int(m_iImageLenth);

		writer.Key("ImageSuffix");
		writer.String(m_strImageSuffix.c_str());

		writer.Key("InnerData");
		writer.String(m_stInnerData.generateJson().c_str());

		writer.EndObject();

		return strbuf.GetString();
	}
}
