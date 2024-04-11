#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QUrl>


namespace module
{
    class GetFile : public QObject, public std::enable_shared_from_this<GetFile>
    {
        Q_OBJECT

    public:
        explicit GetFile(QObject* parent = nullptr);
        ~GetFile();

        void downloadFile(const QUrl& url, const QString& savePath);

    signals:
        void downloadProgress(qint64 bytesRead, qint64 totalBytes);
        void downloadFinished(bool success, const QString& message);

    private:
        QNetworkAccessManager* manager;
    };
}
