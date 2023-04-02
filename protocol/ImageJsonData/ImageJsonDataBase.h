#pragma once

#include "module/stdafx.h"

using namespace boost::property_tree;
using namespace rapidjson;
//all the json data used in communicate should inherit from this class

namespace protocol
{
    class ImageInnerJsonData
    {
    public:
        ImageInnerJsonData(){}
        virtual ~ImageInnerJsonData() {};
        virtual void parse(const std::string& message) {};
        virtual std::string generateJson() { return ""; }
    private:
    };

    class ImageJsonBaseData
    {
    public:
        ImageJsonBaseData()
        {
        }
        virtual ~ImageJsonBaseData()
        {
        }
        ImageJsonBaseData(const ImageJsonBaseData& l) = delete;
        ImageJsonBaseData& operator=(const ImageJsonBaseData& l) = delete;
        virtual void parse(const std::string& message) = 0;
        virtual std::string generateJson() = 0;

        int m_iImageLenth{ 0 };     //ͼƬ�ĳ���
        std::string m_strImageSuffix{ "" };
        ImageInnerJsonData m_stInnerData;   //�ڲ��ɰ���һ�����ݣ���Ϊ��

        MessageType m_strType{ MessageType::InfoWithImage };

    private:
    };
}
