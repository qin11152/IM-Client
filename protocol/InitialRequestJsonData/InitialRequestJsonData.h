#pragma once
#include "../JsonBaseData.h"


class InitialRequestJsonData
    :public JsonBaseData
{
public:
    InitialRequestJsonData(const std::string& message = "");
    void parse(const std::string& message)override;
    std::string generateJson()override;

    //ÓÃ»§µÄid
    std::string m_strId{ "-1" };
    MessageType m_strType{ MessageType::InitialRequest};
};
