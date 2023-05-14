#pragma once
#include "../JsonBaseData.h"

namespace protocol
{
    class LoginInJsonData :
        public JsonBaseData
    {
    public:
        LoginInJsonData(const std::string& message = "");
        void parse(const std::string& message)override;
        std::string generateJson()override;

        std::string m_strId{ "" };
        std::string m_strPassword{ "" };

        MessageType m_strType{ MessageType::LoginRequest };
    };
}
