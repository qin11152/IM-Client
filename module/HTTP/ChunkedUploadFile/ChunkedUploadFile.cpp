#include "ChunkedUploadFile.h"
#include <QFile>

namespace module
{
    ChunkedFileUploader::ChunkedFileUploader(const QString& filePath, const QString& host, quint16 port, const QString& targetPath, QObject* parent)
        : QObject(parent),
        m_filePath(filePath),
        m_host(host), m_port(port),
        m_targetPath(targetPath),
        m_socket(new QTcpSocket(this))
    {
    }
    void ChunkedFileUploader::upload()
    {
        connect(m_socket, &QTcpSocket::connected, this, &ChunkedFileUploader::onConnected);
        connect(m_socket, &QTcpSocket::bytesWritten, this, &ChunkedFileUploader::onBytesWritten);
        connect(m_socket, &QTcpSocket::readyRead, this, &ChunkedFileUploader::onReadyRead);
        m_socket->connectToHost(m_host, m_port);
    }
    void ChunkedFileUploader::onConnected()
    {
        QFile file(m_filePath);
        if (!file.open(QIODevice::ReadOnly))
        {
            qDebug() << "Unable to open file:" << file.errorString();
            return;
        }

        // ����������HTTP����ͷ���������÷ֿ鴫�����
        QByteArray request = "POST " + m_targetPath.toUtf8() + " HTTP/1.1\r\n"
            "Host: " + m_host.toUtf8() + "\r\n"
            "Transfer-Encoding: chunked\r\n"
            "Content-Type: application/octet-stream\r\n\r\n";
        m_socket->write(request);

        // ��ȡ�ļ����ֿ鷢��
        while (!file.atEnd()) {
            QByteArray chunk = file.read(4096); // ��4KBΪһ����
            QByteArray chunkSize = QByteArray::number(chunk.size(), 16) + "\r\n";
            m_socket->write(chunkSize);
            m_socket->write(chunk);
            m_socket->write("\r\n");
        }

        // ���ͽ�����
        m_socket->write("0\r\n\r\n");
    }
    void ChunkedFileUploader::onBytesWritten(qint64 bytes)
    {
        qDebug() << bytes << "bytes written";
    }
    void ChunkedFileUploader::onReadyRead()
    {
        qDebug() << "Response:" << m_socket->readAll();
    }
}