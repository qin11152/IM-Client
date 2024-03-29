﻿#pragma once

#include "../JsonBaseData.h"

namespace protocol
{
    class ProfileImageMsgJsonData :public JsonBaseData
    {
    public:
        ProfileImageMsgJsonData(const std::string& message = "");
        void parse(const std::string& message)override;
        std::string generateJson()override;

        std::string m_strId{ "" };
        std::string m_strUUID{ "" };        //唯一标识，表明是这个图片
        std::string m_strBase64Msg{ "" };   //基于BASE64编码的图片信息
        std::string m_strImageName{ "" };   //图片的名称
        std::string m_strSuffix{ "" };      //图片后缀类型
        std::string m_strTimeStamp{ "" };   //时间戳,发送到服务端就不一样了，所以一并发送过去
        int m_iCurIndex{ 0 };               //当前处于分的第几段
        int m_iSumIndex{ 0 };               //总共分了几段
        ProfileImageType m_eImageType{ ProfileImageType::UpdateProfileImage };  //传递头像的需求

        MessageType m_strType{ MessageType::ProfileImageMsg };
    };
}

