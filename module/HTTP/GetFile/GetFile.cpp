#include "GetFile.h"
#include <qeventloop.h>

namespace module
{
    GetFile::GetFile(QObject* parent)
        : QObject(parent)
    {}

    GetFile::~GetFile()
    {}

    void GetFile::downloadFile(const QUrl& url, const QString& savePath)
    {
        auto self = shared_from_this();
        manager = new QNetworkAccessManager();

        QFile* file = new QFile(savePath);
        if (!file->open(QIODevice::WriteOnly)) {
            emit downloadFinished(false, "Failed to open file for writing");
            delete file;
            file = nullptr;
            return;
        }

        QNetworkReply* reply = manager->get(QNetworkRequest(url));

        QEventLoop loop;

        connect(reply, &QNetworkReply::downloadProgress, this, &GetFile::downloadProgress);

        connect(reply, &QNetworkReply::readyRead, [this, reply, file]() {
            if (file) {
                file->write(reply->readAll()); // 将收到的数据写入文件
            }
            });

        connect(reply, &QNetworkReply::finished, this, [this, reply, &loop, &file]() {
            if (reply->error() == QNetworkReply::NoError) {
                emit downloadFinished(true, "Download completed");
            }
            else 
            {
                emit downloadFinished(false, "Download failed: " + reply->errorString());
            }
            file->close();
            delete file; // 清理资源
            file = nullptr;
            reply->deleteLater();
            loop.exit();
            });

        loop.exec();

        delete manager;
        manager = nullptr;
    }
}