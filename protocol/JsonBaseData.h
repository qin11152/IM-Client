#pragma once

#include "module/stdafx.h"

using namespace boost::property_tree;
using namespace rapidjson;
//all the json data used in communicate should inherit from this class
class JsonBaseData
{
public:
    JsonBaseData()
    {
    }
    ~JsonBaseData()
    {
        std::map<std::string, std::string> clear;
        m_mapMessageInfo.swap(clear);
        m_mapMessageInfo.clear();
    }
    JsonBaseData(const JsonBaseData& l) = delete;
    JsonBaseData& operator=(const JsonBaseData& l) = delete;
    MessageType getType()const { return m_strType; }
    MessageType m_strType{ MessageType::Default };
    virtual void parse(const std::string& message)=0;
    virtual std::string generateJson()=0;
private:
    std::map<std::string, std::string> m_mapMessageInfo;
};
