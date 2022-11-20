#include "ProfileImagePreview.h"
#include "module/PublicDataManager/PublicDataManager.h"
#include "module/TCPThread/TCPThread.h"
#include "module/DataBaseDelegate/DataBaseDelegate.h"
#include "module/Log/Log.h"
#include <QImageReader>
#include <QPainter>
#include <QFileDialog>
#include <QBuffer>
#include <fstream>
#include <chrono>


constexpr QSize kImageCompressSize{ 100,100 };

ProfileImagePreview::ProfileImagePreview(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    //隐藏最大化按钮
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
    //用户点击选择头像按钮
    QFileDialog* fileDialog = new QFileDialog(this);

    //定义文件对话框标题
    fileDialog->setWindowTitle(QString::fromLocal8Bit("选择图片"));

    //设置打开的文件路径
    fileDialog->setDirectory("./");

    //设置文件过滤器,只显示jpg,png, 文件,多个过滤文件使用空格隔开
    fileDialog->setNameFilter(tr("File(*.jpg* *.png* *.jpeg*)"));

    //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles
    fileDialog->setFileMode(QFileDialog::ExistingFiles);

    //设置视图模式
    fileDialog->setViewMode(QFileDialog::Detail);

    fileDialog->show();

    //获取选择的文件的路径
    QStringList fileNames;
    if (fileDialog->exec()) {
        fileNames = fileDialog->selectedFiles();
    }

    if (fileNames.size() > 0)
    {
        m_strPagePath = fileNames[0];
        QImage image(m_strPagePath);
        image = image.scaled(kImageCompressSize, Qt::KeepAspectRatio);
        compressAndSendImage(image);
        saveImageAndUpdateDB(image);
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

void ProfileImagePreview::compressAndSendImage(const QImage& image)
{
    QByteArray byteArr;
    QBuffer buffer(&byteArr);
    QFileInfo info(m_strPagePath);
    QString suffix = info.suffix();//获取文件后缀
    QByteArray stc = suffix.toLatin1();
    image.save(&buffer, stc.data());//将QString类型的后缀名改为char*
    QString str_base64 = byteArr.toBase64();
    QString name = "suibian";
    TCPThread::get_mutable_instance().sendImageMsg(str_base64, name, stc.data());
}

void ProfileImagePreview::saveImageAndUpdateDB(const QImage& image)
{
    QString id = PublicDataManager::get_mutable_instance().getMyId();
    QString lastPath = "";
    DataBaseDelegate::Instance()->queryProfileImagePath(id,lastPath);
    //如果已有路径且已存在图片则删除
    if (!lastPath.isEmpty())
    {
        QFile image(lastPath);
        if (!image.remove())
        {
            _LOG(Logcxx::Level::ERRORS, "delete my last profile image failed");
        }
    }
    QFileInfo info(m_strPagePath);
    QString suffix = info.suffix();//获取文件后缀
    QString curPath = QApplication::applicationDirPath();
    curPath += "/data/image/my." + suffix;
    image.save(curPath);
    auto timet = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    tm timeInfo;
    timeInfo = *localtime(&timet);
    std::stringstream ss;
    ss << std::put_time(&timeInfo, "%F-%T");
    if (DataBaseDelegate::Instance()->queryIsIdExistInProfile(id))
    {
        DataBaseDelegate::Instance()->updateProfilleImagePathAndTimeStamp(id, curPath, ss.str().c_str());
    }
    else
    {
        DataBaseDelegate::Instance()->insertProfilePathAndTimestamp(id, curPath, ss.str().c_str());
    }
}
