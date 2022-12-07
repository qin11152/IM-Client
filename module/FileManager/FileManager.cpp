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

        //Qt���鿴��Ӧ·���ļ����Ƿ���ڣ������ھʹ���
        bool FileManager::checkDirExist(const QString& strDirPath)const
        {
            QDir dir(strDirPath);
            if (!dir.exists())
            {
                return dir.mkpath(strDirPath);
            }
            return true;
        }

        //������Ӧ·���µ��ļ���
        bool FileManager::createDir(const QString& strDirPath)const
        {
            QDir dir(strDirPath);
            return dir.mkpath(strDirPath);
        }

        //�ж��ļ��Ƿ����
        bool FileManager::checkFileExist(const QString& strFilePath)const
        {
            QFileInfo fileInfo(strFilePath);
            return fileInfo.exists();
            return false;
        }

        //���ݲ�������ִ�д����ļ��Ĳ���
        bool FileManager::createFile(const QString& strFilePath, const FileOperateType& operateType) const
        {
            //qt�ļ������ھʹ���
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
                //qt�����ļ�����֮ǰ������
                QFile file(strFilePath);
                return file.open(QIODevice::WriteOnly);
            }
            return false;
        }
    
        //���ļ��ж�ȡ�ļ�����
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
