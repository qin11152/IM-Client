#pragma once

#include "../JsonBaseData.h"

namespace protocol
{
	class StartGroupJsonData : public JsonBaseData
	{
	public:
		StartGroupJsonData(const std::string& message = "");
		~StartGroupJsonData();
		void parse(const std::string& message)override;
		std::string generateJson()override;

		std::string m_strStarterId{ "" };
		std::string m_strGroupName{ "" };
		std::string m_strImagePathInServer{ "" };
		std::vector<std::string> m_vecGroupChat;
		MessageType m_strType{ MessageType::StartGroupChat };
	};
}
