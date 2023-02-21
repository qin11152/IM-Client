#pragma once

#include "protocol/JsonBaseData.h"

class StartGroupChatJsonData : public JsonBaseData
{
public:
    explicit StartGroupChatJsonData(const std::string& message = "");
    void parse(const std::string& message)override;
    std::string generateJson()override;

    std::string m_strStarterId{ "" };
    std::vector<std::string> m_vecGroupChat;

    MessageType m_strType{ MessageType::StartGroupChat };
};

