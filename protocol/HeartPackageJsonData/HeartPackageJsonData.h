#pragma once
#include "../JsonBaseData.h"

namespace protocol
{
    class HeartPackageJsonData :
        public JsonBaseData
    {
    public:
        HeartPackageJsonData(const std::string& message = "");
        virtual void parse(const std::string& message) override;
        virtual std::string generateJson() override;

        MessageType m_strType{ MessageType::HeartPackage };
    private:
    };
}

