/*****************************************************************//**
 * \file   FileManager.h
 * \brief  文件管理类，用于创建文件，文件夹，操作文件等
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
            //不存在就创建
            Default=0,
            //存在就覆盖
            ExistAndOverride,
        };

        class FileManager : public QObject, public boost::serialization::singleton<FileManager>, public boost::noncopyable
        {
            Q_OBJECT

        public:
            FileManager(QObject* parent = nullptr);
            ~FileManager();

            /**
             * brief:查看文件夹是否存在，不存在就创建.
             * param[IN] strDirPath:文件夹路径
             * \return:创建结果
             */
            bool checkDirExist(const QString& strDirPath)const;

            /**
             * brief:创建对应路径下的文件夹.
             * 
             * \param strDirPath：文件夹路径
             * \return true:成功
             */
            bool createDir(const QString& strDirPath)const;

            /**
             * brief:判断文件是否存在.
             * 
             * \param strFilePath：文件路径
             * \return true：成功
             */
            bool checkFileExist(const QString& strFilePath)const;
            

            /**
             * brief:读取对应路径中的文件内容.
             * 
             * \param strFilePath:文件路径
             * \param fileInfo：读取内容写入该字符串
             */
            void readFile(const QString& strFilePath, QString& fileInfo)const;

            /**
             * brief:根据传入的模式创建文件.
             *
             * \param operateType[IN]:操作类型
             * \param strFilePath[IN]:文件路径
             * \return 创建结果
             */
            bool createFile(const QString& strFilePath, const FileOperateType& operateType=FileOperateType::Default)const;

        };
    }
}




