#include "host.h"

#include <QHBoxLayout>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QRegExp>

#include "resources/lib.h"

namespace Web {

Host::Host(QWidget *parent)
    : QDialog(parent)
    , listener_(0)
    , request_handler_(0)
    , address_()
    , line_edit_(0)
    , chat_app_(0)
    , logger_("Web::Host")
{
    initListener();
    initWidgets();
    initLayout();
}

Host::~Host()
{
    delete listener_;
    delete request_handler_;
    listener_ = 0;
    request_handler_ = 0;
}

const QString &Host::getAddress() const
{
    return address_;
}

void Host::setPresetView(Tile::GraphicsView *preset_view)
{
    request_handler_->setPresetView(preset_view);
}

void Host::initListener()
{
    QSettings* settings = new QSettings(
        Resources::Lib::WEB_CONFIG,
        QSettings::IniFormat,
        this
    );
    settings->beginGroup("listener");

    request_handler_ = new RequestHandler(this);

    listener_ = new stefanfrings::HttpListener(
        settings,
        request_handler_
    );

    logger_.write("SUCCESS, Started Web host.");
}

void Host::initWidgets()
{
    address_ = generateWebHostLinks();

    line_edit_ = new QLineEdit(address_, this);
    line_edit_->setReadOnly(true);

    chat_app_ = new App::Chat(request_handler_->getChatMessageModel(), this);
    image_app_ = new App::ImageLoader(this);

    connect(image_app_, SIGNAL(newImageLoaded(QString)),
            request_handler_, SLOT(onNewImageLoaded(QString)));
}

void Host::initLayout()
{
    setMinimumSize(600, 400);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(line_edit_, 0);
    layout->addWidget(chat_app_, 10);
    layout->addWidget(image_app_, 1);

    setLayout(layout);
}

const QString Host::generateWebHostLinks()
{
    QString links = "";
    foreach(const QNetworkInterface& interface, QNetworkInterface::allInterfaces()) {
        if(!(interface.flags() & QNetworkInterface::IsLoopBack) && interface.flags() & QNetworkInterface::IsUp) {
            foreach(QNetworkAddressEntry entry, interface.addressEntries()) {
                if(entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    if(links.size() != 0)
                        links += ", ";
                    QString ip_formatted = entry.ip().toString();
                    ip_formatted.replace(".", "-");
                    links += Resources::Lib::WEB_URL_PREFIX;
                    links += ip_formatted;
                }
            }
        }
    }
    return links;
}

} // namespace Web
