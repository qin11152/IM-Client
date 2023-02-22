#include "../JsonBaseData.h"

namespace protocol
{
    class RegisterReplyData :public JsonBaseData
    {
    private:
        /* data */
    public:
        RegisterReplyData(const std::string& message = "");
        ~RegisterReplyData() {}
        virtual void parse(const std::string& message)override;
        virtual std::string generateJson()override;

        bool m_bRegisterResult{ false };
        //�û�ע���õ����Լ���id
        int m_iId{ -1 };
        MessageType m_strType{ MessageType::RegisterResponse };
    };
}

