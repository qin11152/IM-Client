#pragma once
#include "protocol/JsonBaseData.h"

namespace protocol
{
	class GroupChatJsonData : public JsonBaseData
	{
	public:
		GroupChatJsonData(const std::string& message);
		void parse(const std::string& message) override;
		std::string generateJson() override;

		std::string m_strSendId{ "" };
		std::string m_strMsg{ "" };

		MessageType m_strType{ MessageType::GroupChat };
	};
}

