#pragma once
#include "protocol/JsonBaseData.h"

namespace protocol
{
    class StartGroupChatReplyJsonData :public JsonBaseData
    {
    public:
        explicit StartGroupChatReplyJsonData(const std::string& message = "");
        void parse(const std::string& message)override;
        std::string generateJson()override;

        std::string m_strGroupChatId{ "" };
        std::string m_strGuid{ "" };
        bool m_bIsSuccess{ false };

        MessageType m_strType{ MessageType::StartGroupChatReply };
    };
}
