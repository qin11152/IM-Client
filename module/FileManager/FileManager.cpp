#include "FileManager.h"
#include <QDir>


namespace Base
{
    namespace fileoperate
    {
        FileManager::FileManager(QObject* parent)
            : QObject(parent)
        {}

        FileManager::~FileManager()
        {}

        //Qt风格查看对应路径文件夹是否存在，不存在就创建
        bool FileManager::checkDirExist(const QString& strDirPath)const
        {
            QDir dir(strDirPath);
            if (!dir.exists())
            {
                return dir.mkpath(strDirPath);
            }
            return true;
        }

        //创建对应路径下的文件夹
        bool FileManager::createDir(const QString& strDirPath)const
        {
            QDir dir(strDirPath);
            return dir.mkpath(strDirPath);
        }

        //判断文件是否存在
        bool FileManager::checkFileExist(const QString& strFilePath)const
        {
            QFileInfo fileInfo(strFilePath);
            return fileInfo.exists();
            return false;
        }

        //根据操作类型执行创建文件的操作
        bool FileManager::createFile(const QString& strFilePath, const FileOperateType& operateType) const
        {
            //qt文件不存在就创造
            if (operateType == FileOperateType::Default)
            {
                QFile file(strFilePath);
                if (!file.exists())
                {
                    return file.open(QIODevice::WriteOnly);
                }
            }
            else
            {
                //qt创建文件覆盖之前的内容
                QFile file(strFilePath);
                return file.open(QIODevice::WriteOnly);
            }
            return false;
        }
    
        //从文件中读取文件内容
        void FileManager::readFile(const QString& strFilePath, QString& fileInfo) const
        {
            QFile file(strFilePath);
            if (file.open(QIODevice::ReadOnly))
            {
                fileInfo = file.readAll();
                file.close();
            }
        }
    }
}
