#include "ChunkedUploadFile.h"
#include <QHttpMultiPart>
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

    void FileUploader::uploadFile(const QString& url, const QString& filePath, const QString& formFieldName)
    {
        auto self = shared_from_this();
        manager = new QNetworkAccessManager(this);
        QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);

        QHttpPart filePart;
        filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"" + formFieldName + "\"; filename=\"" + QFileInfo(filePath).fileName() + "\""));
        QMimeType mimeType = QMimeDatabase().mimeTypeForFile(filePath);
        filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(mimeType.name()));
        QFile* file = new QFile(filePath, this);
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

        connect(reply, &QNetworkReply::finished, this, [this, reply, &loop]() {
            if (reply->error() == QNetworkReply::NoError) {
                emit uploadFinished(true, reply->readAll());
            }
            else {
                emit uploadFinished(false, reply->errorString());
            }
            loop.exit();
            reply->deleteLater();
            });
        loop.exec();
    }
}