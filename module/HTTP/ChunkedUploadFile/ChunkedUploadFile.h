#pragma once
#include "protocol/JsonBaseData.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QHttpMultiPart>
#include <QNetworkReply>
#include <QFile>

#include <functional>


namespace module
{
    using TaskForResponse=std::function<void(std::shared_ptr<protocol::JsonBaseData> jsondataPtr, const std::string&)>;

    class FileUploader : public QObject,public std::enable_shared_from_this<FileUploader>
    {
        Q_OBJECT

    public:
        explicit FileUploader(QObject* parent = nullptr);
        ~FileUploader();
        void uploadFile(const QString& url, const QString& filePath);
        void uploadFileWithJson(const QString& url, const QString& filePath, TaskForResponse task);

    signals:
        void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
        void uploadFinished(bool success, const QString& response);

    private:
        QNetworkAccessManager* manager;
    };
}
