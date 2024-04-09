#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

namespace module
{


    class FileUploader : public QObject,public std::enable_shared_from_this<FileUploader>
    {
        Q_OBJECT

    public:
        explicit FileUploader(QObject* parent = nullptr);
        ~FileUploader();
        void uploadFile(const QString& url, const QString& filePath, const QString& formFieldName = "file");

    signals:
        void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
        void uploadFinished(bool success, const QString& response);

    private:
        QNetworkAccessManager* manager;
    };
}
