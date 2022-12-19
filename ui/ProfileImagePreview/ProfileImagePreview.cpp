#include "ProfileImagePreview.h"
#include "module/Log/Log.h"
#include "module/TCPThread/TCPThread.h"
#include "module/PublicFunction/PublicFunction.h"
#include "module/PublicDataManager/PublicDataManager.h"
#include "module/DataBaseDelegate/DataBaseDelegate.h"

#include <QImageReader>
#include <QPainter>
#include <QFileDialog>
#include <QBuffer>


constexpr QSize kImageCompressSize{ 100,100 };

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
        QImage image(m_strPagePath);
        image = image.scaled(kImageCompressSize, Qt::KeepAspectRatio);
        std::string timeStamp = Base::timeToString("%F-%T");
        compressAndSendImage(image, timeStamp);
        saveImageAndUpdateDB(image, timeStamp);
        //��Ϊ���Լ���ͷ���ˣ�����id���Լ���id
        emit signalProfileImageChanged(PublicDataManager::get_mutable_instance().getMyId(), m_strPagePath);
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
}

void ProfileImagePreview::compressAndSendImage(const QImage& image, const std::string& timeStamp)
{
    QByteArray byteArr;
    QBuffer buffer(&byteArr);
    QFileInfo info(m_strPagePath);
    QString suffix = info.suffix();//��ȡ�ļ���׺
    QByteArray stc = suffix.toLatin1();
    image.save(&buffer, stc.data());//��QString���͵ĺ�׺����Ϊchar*
    QString str_base64 = byteArr.toBase64();
    QString name = PublicDataManager::get_mutable_instance().getMyId();
    TCPThread::get_mutable_instance().sendImageMsg(str_base64, name, stc.data(), timeStamp.c_str());
}

void ProfileImagePreview::saveImageAndUpdateDB(const QImage& image, const std::string& timeStamp)
{
    QString id = PublicDataManager::get_mutable_instance().getMyId();
    QString lastPath = "";
    DataBaseDelegate::Instance()->queryProfileImagePath(id,lastPath);
    //�������·�����Ѵ���ͼƬ��ɾ��
    if (!lastPath.isEmpty())
    {
        QFile image(lastPath);
        if (!image.remove())
        {
            _LOG(Logcxx::Level::ERRORS, "delete my last profile image failed");
        }
    }
    QFileInfo info(m_strPagePath);
    QString suffix = info.suffix();//��ȡ�ļ���׺
    QString savePath = PublicDataManager::get_mutable_instance().getIdDirPath() + "/image/" + id + "." + suffix;
    PublicDataManager::get_mutable_instance().setImagePath(savePath);
    image.save(savePath);
    if (DataBaseDelegate::Instance()->queryIsIdExistInProfile(id))
    {
        DataBaseDelegate::Instance()->updateProfilleImagePathAndTimeStamp(id, savePath, timeStamp.c_str());
    }
    else
    {
        DataBaseDelegate::Instance()->insertProfilePathAndTimestamp(id, savePath, timeStamp.c_str());
    }
}
