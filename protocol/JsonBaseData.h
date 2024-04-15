#pragma once

#include "module/stdafx.h"

using namespace boost::property_tree;
using namespace rapidjson;
//all the json data used in communicate should inherit from this class

namespace protocol
{
    class JsonBaseData
    {
    public:
        JsonBaseData()
        {
        }
        ~JsonBaseData()
        {
        }
        //JsonBaseData(const JsonBaseData& l) = delete;
        //JsonBaseData& operator=(const JsonBaseData& l) = delete;
        MessageType getType()const { return m_strType; }
        MessageType m_strType{ MessageType::Default };
        virtual void parse(const std::string& message) {}
        virtual std::string generateJson() { return ""; }
    };
}
