#pragma once

#include "../JsonBaseData.h"

namespace protocol
{
    class getProfileImageJsonData :
        public JsonBaseData
    {
    public:
        getProfileImageJsonData(const std::string& message = "");
        void parse(const std::string& message)override;
        std::string generateJson()override;

        std::string m_strId{ "" };

        MessageType m_strType{ MessageType::getFriendProfileImage };
    };
}

