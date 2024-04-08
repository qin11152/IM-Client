#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QIODevice>
#include <QByteArray>

namespace module
{
    class ChunkedFileUploader : public QObject
    {
        Q_OBJECT
    public:
        ChunkedFileUploader(const QString& filePath, const QString& host, quint16 port, const QString& targetPath, QObject* parent = nullptr);

        void upload(); 

    private:
        void onConnected();

        void onBytesWritten(qint64 bytes);

        void onReadyRead();

    private:
        QTcpSocket* m_socket;
        QString m_filePath;
        QString m_host;
        quint16 m_port;
        QString m_targetPath;
    };
}
