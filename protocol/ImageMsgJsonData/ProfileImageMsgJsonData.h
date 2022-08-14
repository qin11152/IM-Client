#pragma once
#include "../JsonBaseData.h"
class ProfileImageMsgJsonData :
    public JsonBaseData
{
public:
    ProfileImageMsgJsonData(const std::string& message = "");
    void parse(const std::string& message)override;
    std::string generateJson()override;

    std::string m_strId{ "" };
    std::string m_strUUID{ "" };        //Ψһ��ʶ�����������ͼƬ
    std::string m_strBase64Msg{ "" };   //����BASE64�����ͼƬ��Ϣ
    std::string m_strImageName{ "" };   //ͼƬ������
    int m_iCurIndex{ 0 };              //��ǰ���ڷֵĵڼ���
    int m_iSumIndex{ 0 };               //�ܹ����˼���

    MessageType m_strType{ MessageType::ProfileImageMsg };
};
