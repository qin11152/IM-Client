/*****************************************************************//**
 * \file   FileManager.h
 * \brief  �ļ������࣬���ڴ����ļ����ļ��У������ļ���
 * 
 * \author Administrator
 * \date   December 2022
 *********************************************************************/
#pragma once

#include <QObject>
#include <boost/serialization/singleton.hpp>

namespace Base
{
    namespace fileoperate
    {
        enum class FileOperateType
        {
            //�����ھʹ���
            Default=0,
            //���ھ͸���
            ExistAndOverride,
        };

        class FileManager : public QObject, public boost::serialization::singleton<FileManager>, public boost::noncopyable
        {
            Q_OBJECT

        public:
            FileManager(QObject* parent = nullptr);
            ~FileManager();

            /**
             * brief:�鿴�ļ����Ƿ���ڣ������ھʹ���.
             * param[IN] strDirPath:�ļ���·��
             * \return:�������
             */
            bool checkDirExist(const QString& strDirPath)const;

            /**
             * brief:������Ӧ·���µ��ļ���.
             * 
             * \param strDirPath���ļ���·��
             * \return true:�ɹ�
             */
            bool createDir(const QString& strDirPath)const;

            /**
             * brief:�ж��ļ��Ƿ����.
             * 
             * \param strFilePath���ļ�·��
             * \return true���ɹ�
             */
            bool checkFileExist(const QString& strFilePath)const;
            

            /**
             * brief:��ȡ��Ӧ·���е��ļ�����.
             * 
             * \param strFilePath:�ļ�·��
             * \param fileInfo����ȡ����д����ַ���
             */
            void readFile(const QString& strFilePath, QString& fileInfo)const;

            /**
             * brief:���ݴ����ģʽ�����ļ�.
             *
             * \param operateType[IN]:��������
             * \param strFilePath[IN]:�ļ�·��
             * \return �������
             */
            bool createFile(const QString& strFilePath, const FileOperateType& operateType=FileOperateType::Default)const;

        };
    }
}




