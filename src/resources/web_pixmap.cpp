#include "web_pixmap.h"

WebPixmap::WebPixmap(QObject *parent)
    : QObject(parent)
    , pixmap_()
    , network_manager_(0)
    , url_()
{
    network_manager_ = new QNetworkAccessManager(this);
    connect(network_manager_, &QNetworkAccessManager::finished,
            this, &WebPixmap::onManagerFinished);
}

void WebPixmap::setUrl(const QUrl &url)
{
    url_ = url;
    pixmap_ = QPixmap();
    requestPixmap();
}

const QUrl &WebPixmap::getUrl() const
{
    return url_;
}

const QPixmap &WebPixmap::getPixmap() const
{
    return pixmap_;
}

void WebPixmap::onManagerFinished(QNetworkReply* reply)
{
    QPixmap new_pixmap;
    if(!new_pixmap.loadFromData(reply->readAll()))
        return;
    pixmap_ = new_pixmap;
    emit contentChanged();
}

void WebPixmap::requestPixmap()
{
    QNetworkRequest request(url_);
    network_manager_->get(request);
}
