#include "ProfileImagePreview.h"
#include "module/PublicDataManager/PublicDataManager.h"
#include "module/TCPThread/TCPThread.h"
#include <QImageReader>
#include <QPainter>
#include <QFileDialog>
#include <QBuffer>
#include <fstream>

ProfileImagePreview::ProfileImagePreview(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    //������󻯰�ť
    setWindowFlag(Qt::WindowMaximizeButtonHint, true);
    setFixedSize(QSize(400,400));
    setWindowTitle("");

    initConnect();
}

ProfileImagePreview::~ProfileImagePreview()
{}

void ProfileImagePreview::setImagePath(const QString& path, const int id)
{
    m_strPagePath = path;
    m_iPageId = id;
}

void ProfileImagePreview::onSignalChooseBtnClicked()
{
    //�û����ѡ��ͷ��ť
    QFileDialog* fileDialog = new QFileDialog(this);

    //�����ļ��Ի������
    fileDialog->setWindowTitle(QString::fromLocal8Bit("ѡ��ͼƬ"));

    //���ô򿪵��ļ�·��
    fileDialog->setDirectory("./");

    //�����ļ�������,ֻ��ʾjpg,png, �ļ�,��������ļ�ʹ�ÿո����
    fileDialog->setNameFilter(tr("File(*.jpg* *.png* *.jpeg*)"));

    //���ÿ���ѡ�����ļ�,Ĭ��Ϊֻ��ѡ��һ���ļ�QFileDialog::ExistingFiles
    fileDialog->setFileMode(QFileDialog::ExistingFiles);

    //������ͼģʽ
    fileDialog->setViewMode(QFileDialog::Detail);

    fileDialog->show();

    //��ȡѡ����ļ���·��
    QStringList fileNames;
    if (fileDialog->exec()) {
        fileNames = fileDialog->selectedFiles();
    }

    if (fileNames.size() > 0)
    {
        m_strPagePath = fileNames[0];
        //TODO ���͸����������޸ı��ش洢��
        QImage image(m_strPagePath);
        image = image.scaled(100, 100);
        QByteArray byteArr;
        QBuffer buffer(&byteArr);
        QFileInfo info(m_strPagePath);
        QString suffix = info.suffix();//��ȡ�ļ���׺
        QByteArray stc = suffix.toLatin1();
        image.save(&buffer, stc.data());//��QString���͵ĺ�׺����Ϊchar*
        QString str_base64 = byteArr.toBase64();
        QString name = "suibian";
        TCPThread::get_mutable_instance().sendImageMsg(str_base64, name, stc.data());
        //emit signalSendImageMsg(str_base64, name, stc.data());
        QString curPath=QApplication::applicationDirPath();
        curPath += "/data/image/my." + stc;
        image.save(curPath);
        update();
    }
}

void ProfileImagePreview::onSignalUpdateProfileImage()
{
}

void ProfileImagePreview::paintEvent(QPaintEvent* event)
{
    if(m_strPagePath.isEmpty())
    {
        m_strPagePath = kDefaultProfileImageWidget;
    }
    QRect rect = ui.widget->rect();
    QPainter p(this);
    QImageReader reader;
    QImage image;
    reader.setDecideFormatFromContent(true);
    reader.setScaledSize(QSize(rect.width(),rect.height()));
    reader.setFileName(m_strPagePath);
    if (reader.canRead())
    {
        if (!reader.read(&image))
        {
            QImageReader::ImageReaderError error = reader.error();
            QString strError = reader.errorString();
            printf("last error:%s\n", strError.toStdString().c_str());
            return;
        }
    }
    p.drawPixmap(40,50,330,330, QPixmap::fromImage(image)/*.scaled(width(), height(), Qt::KeepAspectRatio)*/);
}

void ProfileImagePreview::showEvent(QShowEvent* event)
{
    if(m_iPageId!=PublicDataManager::get_mutable_instance().getMyId().toInt())
    {
        ui.changeProfileImagePushButton->hide();
    }
    else
    {
        ui.changeProfileImagePushButton->show();
    }
}

void ProfileImagePreview::initConnect()
{
    connect(ui.changeProfileImagePushButton, &QPushButton::clicked, this, &ProfileImagePreview::onSignalChooseBtnClicked);
    connect(this, &ProfileImagePreview::signalSendImageMsg, &TCPThread::get_mutable_instance(), &TCPThread::sendImageMsg, Qt::QueuedConnection);
}
