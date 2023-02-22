#include "../JsonBaseData.h"

namespace protocol
{
    class RegisterJsonData :public JsonBaseData
    {
    public:
        RegisterJsonData(const std::string& message = "");
        virtual void parse(const std::string& message) override;
        virtual std::string generateJson() override;

        std::string m_strUserName{ "" };
        std::string m_strUserPassword{ "" };
        MessageType m_strType{ MessageType::RegisterRequest };
    private:
    };
}

