#include "ChunkedUploadFile.h"
#include <QMimeDatabase>
#include <QFileInfo>
#include <QEventLoop>

namespace module
{
    FileUploader::FileUploader(QObject* parent) : QObject(parent)
    {
    }

    FileUploader::~FileUploader()
    {
    }

    void FileUploader::uploadFile(const QString& url, const QString& filePath)
    {
        auto self = shared_from_this();
        manager = new QNetworkAccessManager();
        QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

        QHttpPart textPart;
        textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"description\""));
        textPart.setBody("This is a text part.");

        multiPart->append(textPart);

        QHttpPart filePart;
        filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\"" + QFileInfo(filePath).fileName() + "\""));
        QMimeType mimeType = QMimeDatabase().mimeTypeForFile(filePath);
        filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(mimeType.name()));
        QFile* file = new QFile(filePath);
        if (!file->open(QIODevice::ReadOnly)) {
            emit uploadFinished(false, "Failed to open file");
            return;
        }
        filePart.setBodyDevice(file);

        multiPart->append(filePart);

        QNetworkRequest request{ QUrl(url) };

        QEventLoop loop;
        QNetworkReply* reply = manager->post(request, multiPart);
        multiPart->setParent(reply); // 确保multiPart的生命周期

        connect(reply, &QNetworkReply::uploadProgress, this, &FileUploader::uploadProgress, Qt::QueuedConnection);

        connect(reply, &QNetworkReply::finished, this, [this, reply, &loop, file]() {
            if (reply->error() == QNetworkReply::NoError) {
                emit uploadFinished(true, reply->readAll());
            }
            else {
                emit uploadFinished(false, reply->errorString());
            }
            loop.exit();
            reply->deleteLater();
            file->deleteLater();
            });
        loop.exec();
        delete manager;
        manager = nullptr;
    }
}