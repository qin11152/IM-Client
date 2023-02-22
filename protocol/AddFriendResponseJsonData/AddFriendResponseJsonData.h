#pragma once
#include "../JsonBaseData.h"

namespace protocol
{
    class AddFriendResponseJsonData :public JsonBaseData
    {
    public:
        AddFriendResponseJsonData(const std::string& message = "");
        void parse(const std::string& message)override;
        std::string generateJson()override;

        //ÓÃ»§µÄid
        std::string m_strMyId{ "-1" };
        std::string m_strFriendId{ "-1" };
        bool m_bResult{ false };
        MessageType m_strType{ MessageType::AddFriendResponse };
    };
}

