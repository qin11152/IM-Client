#include "module/stdafx.h"
#include "RegisterWidget.h"
#include "ui_RegisterWidget.h"
#include "protocol/RegisterJsonData/RegisterJsonData.h"
#include "protocol/RegisterReplyData/RegisterReplyData.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QCloseEvent>
#include <QJsonDocument>
#include <QJsonParseError>

using namespace module;

namespace wechat
{
    RegisterWidget::RegisterWidget(QWidget* parent)
        : QWidget(parent)
    {
        ui = new Ui::RegisterWidget();
        ui->setupUi(this);
        initUi();
        initConnect();
    }

    void RegisterWidget::cleanLineEdit()
    {
        ui->firstPasswordLineEdit->clear();
        ui->secondPasswordLineEdit->clear();
        ui->nickLineEdit->clear();
    }

    void RegisterWidget::closeEvent(QCloseEvent* event)
    {
        emit signalShowLoginInWidget();
        event->accept();
    }

    void RegisterWidget::onNickLineEditEditingFinished(const QString& text)
    {
    }

    void RegisterWidget::onfirstPasswordLineEditEditingFinished(const QString& text)
    {
        QRegularExpression rx("^(?![0-9]+$)(?![a-zA-Z]+$)[0-9A-Za-z]{8,}$");
        //QRegExp rx("^(?!(?:[^a-zA-Z]+|\D+|[a-zA-Z0-9]+)$).{8,}$");
        //得到比对结果
        auto machResult = rx.match(text);
        if (!machResult.hasMatch())
        {
            ui->label_2->setVisible(true);
        }
        else
        {
            ui->label_2->setVisible(false);
        }

        if (!ui->secondPasswordLineEdit->text().isEmpty())
        {
            if (ui->secondPasswordLineEdit->text() != text)
            {
                ui->label->setVisible(true);
            }
            else
            {
                ui->label->setVisible(false);
            }
        }

        if (text.isEmpty())
        {
            ui->label_2->setVisible(false);
        }
    }

    void RegisterWidget::onsecondPasswordLineEditEditingFinished(const QString& text)
    {
        if (text != ui->firstPasswordLineEdit->text())
        {
            ui->label->setVisible(true);
        }
        else
        {
            ui->label->setVisible(false);
        }
        if (text.isEmpty() && ui->firstPasswordLineEdit->text().isEmpty())
        {
            ui->label->setVisible(false);
        }
    }

    void RegisterWidget::onRegisterClicked()
    {
        //qDebug() << "clicked";
        if (ui->firstPasswordLineEdit->text().isEmpty() || ui->secondPasswordLineEdit->text().isEmpty()
            || ui->nickLineEdit->text().isEmpty() || ui->label->isVisible() || ui->label_2->isVisible())
        {
            return;
        }
        else
        {
            //qDebug() << "edit content is:" << ui->nickLineEdit->text() << "  " << ui->firstPasswordLineEdit->text();
            protocol::RegisterJsonData reg("");
            reg.m_strUserName = ui->nickLineEdit->text().toStdString();
            reg.m_strUserPassword = ui->firstPasswordLineEdit->text().toStdString();
            reg.m_strType = MessageType::RegisterRequest;
            //qDebug() << "reg content is:" << reg.m_strUserName.c_str() << "  " << reg.m_strUserPassword.c_str();
            std::string registerMessage = reg.generateJson();
            TCPOperateInterface::get_mutable_instance().sendMessageExternalInterface(registerMessage);
            //emit signalSendMsg(registerMessage);
            return;
        }
    }

    void RegisterWidget::onMsgHandle(const QString& msg)
    {
        //printf(msg.toStdString().c_str());
        ptree m_ptree;
        std::stringstream sstream(msg.toStdString().c_str());
        read_json(sstream, m_ptree);
        int messageType = m_ptree.get<int>("type");
        qDebug() << messageType;
        if (static_cast<MessageTypeBaseType>(MessageType::RegisterResponse) == messageType)
        {
            const bool registerResult = m_ptree.get<bool>("RegisterResult");
            const int id = m_ptree.get<int>("UserId");
            if (registerResult == true)
            {
                const QString tmp = QString::fromLocal8Bit("注册成功,您的id为") + QString::number(id) + QString::fromLocal8Bit("请牢记");
                switch (QMessageBox::information(this, QString::fromLocal8Bit("注册结果"),
                    tmp, QMessageBox::Ok), 0)
                {
                case 0:
                {
                    hide();
                    emit signalShowLoginInWidget();
                    break;
                }
                default:
                    break;
                }
            }
            else
            {
                QMessageBox::warning(this, QString::fromLocal8Bit("注册失败"),
                    QString::fromLocal8Bit("未能成功注册，请再次尝试"),
                    QMessageBox::Ok);
            }
        }
    }

    void RegisterWidget::onSignalConnectedFailed()
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("无法连接服务器"),
            QString::fromLocal8Bit("未能与服务器建立连接，请调整网络后重试"),
            QMessageBox::Ok);
    }

    void RegisterWidget::initConnect()
    {
        connect(ui->registerButton, &QPushButton::clicked, this, &RegisterWidget::onRegisterClicked);
        connect(ui->nickLineEdit, &QLineEdit::textChanged, this, &RegisterWidget::onNickLineEditEditingFinished);
        connect(ui->firstPasswordLineEdit, &QLineEdit::textChanged, this, &RegisterWidget::onfirstPasswordLineEditEditingFinished);
        connect(ui->secondPasswordLineEdit, &QLineEdit::textChanged, this, &RegisterWidget::onsecondPasswordLineEditEditingFinished);
        connect(&TCPOperateInterface::get_mutable_instance(), &TCPOperateInterface::signalRecvRegisterMessage, this, &RegisterWidget::onMsgHandle, Qt::QueuedConnection);
        connect(&TCPOperateInterface::get_mutable_instance(), &TCPOperateInterface::signalConnectFailed, this, &RegisterWidget::onSignalConnectedFailed, Qt::QueuedConnection);
    }

    void RegisterWidget::initUi()
    {
        ui->label->setVisible(false);
        ui->label_2->setVisible(false);
        setWindowFlag(Qt::WindowStaysOnTopHint);
        setWindowTitle("注册");
    }

    RegisterWidget::~RegisterWidget()
    {
        delete ui;
    }
}