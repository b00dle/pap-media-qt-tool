#include "socket_host_widget.h"

#include <QHBoxLayout>
#include <QMessageBox>

SocketHostWidget::SocketHostWidget(Tile::GraphicsView* view, QWidget *parent)
    : QWidget(parent)
    , server_(0)
    , udp_discovery_(0)
    , text_edit_(0)
    , name_edit_(0)
{
    server_ = new CompanionServer(41736, this);
    server_->setGraphicsView(view);
    udp_discovery_ = new CompanionUdpDiscovery(41736, "Companion Host", this);
    initWidgets();
    initLayout();
}

void SocketHostWidget::onNameSubmit()
{
    QString curr_name = udp_discovery_->getDiscoveryName();
    if(curr_name.compare(name_edit_->text()) == 0)
        return;
    QMessageBox b;
    b.setText(tr("You are about to change your server name from '") + curr_name + tr("' to '") + name_edit_->text());
    b.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    b.setDefaultButton(QMessageBox::Cancel);
    if(b.exec() == QMessageBox::Ok)
        udp_discovery_->setDiscoveryName(name_edit_->text());
    else
        name_edit_->setText(udp_discovery_->getDiscoveryName());
}

void SocketHostWidget::initWidgets()
{
    text_edit_ = new QTextEdit(this);
    text_edit_->setReadOnly(true);
    connect(server_, &CompanionServer::messageSent,
            [=](const QByteArray& arr)
        {
            text_edit_->setText(QString(arr));
        }
    );

    name_edit_ = new QLineEdit(udp_discovery_->getDiscoveryName(), this);
    name_submit_ = new QPushButton(tr("Submit"), this);
    connect(name_submit_, &QPushButton::clicked,
            this, &SocketHostWidget::onNameSubmit);
}

void SocketHostWidget::initLayout()
{
    QVBoxLayout* layout = new QVBoxLayout;

    QHBoxLayout* upper_layout = new QHBoxLayout;
    upper_layout->setContentsMargins(0,0,0,0);
    upper_layout->addWidget(name_edit_, 10);
    upper_layout->addWidget(name_submit_, -1);

    layout->addLayout(upper_layout, -1);
    layout->addWidget(text_edit_);
    setLayout(layout);
    setWindowTitle("Companion Server");
}
