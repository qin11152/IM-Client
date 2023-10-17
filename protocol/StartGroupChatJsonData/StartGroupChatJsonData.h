#pragma once

#include "protocol/ImageJsonData/ImageJsonDataBase.h"

namespace protocol
{
	class StartGroupChatInnerData : public ImageInnerJsonData
	{
	public:
		StartGroupChatInnerData(const std::string& message = "");
		~StartGroupChatInnerData()override;
		void parse(const std::string& message)override;
		std::string generateJson()override;

		std::string m_strStarterId{ "" };
		std::string m_strGuid{ "" };
		std::string m_strGroupName{ "" };
		std::vector<std::string> m_vecGroupChat;

		MessageType m_strType{ MessageType::StartGroupChat };

	};

	class StartGroupJsonData : public ImageJsonBaseData
	{
	public:
		StartGroupJsonData(const std::string& message = "");
		~StartGroupJsonData()override;
		void parse(const std::string& message)override;
		std::string generateJson()override;

		StartGroupChatInnerData m_stInnerData;   //内部可包含一层数据，可为空
	};
}
