﻿#include "AddFriendNotifyJsonData.h"

namespace protocol
{
    AddFriendNotify::AddFriendNotify(const std::string& message)
    {
        parse(message);
    }

    void AddFriendNotify::parse(const std::string& message)
    {
        if (message.empty())
        {
            return;
        }
        ptree m_ptree;
        std::stringstream sstream(message);
        read_json(sstream, m_ptree);
        m_strId1 = m_ptree.get<std::string>("Id1");
        m_strId2 = m_ptree.get<std::string>("Id2");
        m_strName1 = m_ptree.get<std::string>("Name1");
        m_strName2 = m_ptree.get<std::string>("Name2");
        m_strImageStamp1 = m_ptree.get<std::string>("TimeStamp1");
        m_strImageStamp2 = m_ptree.get<std::string>("TimeStamp2");
        return;
    }

    std::string AddFriendNotify::generateJson()
    {
        ptree m_ptree;

        m_ptree.put("Id1", m_strId1.c_str());
        m_ptree.put("Id2", m_strId2.c_str());
        m_ptree.put("Name1", m_strName1.c_str());
        m_ptree.put("Name2", m_strName2.c_str());
        m_ptree.put("TimeStamp1", m_strImageStamp1.c_str());
        m_ptree.put("TimeStamp2", m_strImageStamp2.c_str());

        std::stringstream sstream;
        write_json(sstream, m_ptree);
        return sstream.str();
    }
}

