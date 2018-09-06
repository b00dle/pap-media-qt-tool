
#include "oauth2_request_handler.h"

#include <QNetworkReply>

OAuth2RequestHandler::OAuth2RequestHandler(QObject *parent)
    : QObject(parent)
    , network_manager_(0)
    , request_()
    , access_granted_(false)
{
    network_manager_ = new  QNetworkAccessManager();
    connect(network_manager_, &QNetworkAccessManager::finished,
            this, &OAuth2RequestHandler::requestFinished);
}

OAuth2RequestHandler::~OAuth2RequestHandler()
{
    network_manager_->deleteLater();
}

QNetworkReply *OAuth2RequestHandler::get(const QString &url) {
    QUrl u(base_url + url);
    request_.setUrl(u);
    return network_manager_->get(request_);
}

QNetworkReply *OAuth2RequestHandler::put(const QString &url, const QByteArray &parameter) {
    QUrl u(base_url + url);
    request_.setUrl(u);
    return network_manager_->put(request_, parameter);
}

QNetworkReply *OAuth2RequestHandler::put(const QString &url) {
    QUrl u(base_url + url);
    request_.setUrl(u);
    return network_manager_->put(request_, QByteArray());
}

QNetworkReply *OAuth2RequestHandler::post(const QString &url, const QByteArray &parameter) {
    request_.setUrl(base_url + url);
    request_.setUrl(base_url + url);
    return  network_manager_->post(request_, parameter);
}

QNetworkReply *OAuth2RequestHandler::post(const QString &url) {
    request_.setUrl(base_url + url);
    request_.setUrl(base_url + url);
    return network_manager_->post(request_, QByteArray());
}

void OAuth2RequestHandler::setRequestHeaders(const QString &token) {
    request_ = QNetworkRequest();

    QString bearer = token;
    request_.setRawHeader("Authorization", "Bearer " + bearer.toUtf8());
    request_.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    QString user_agent = QStringLiteral("QtOAuth/1.0 (+https://www.qt.io)");
    request_.setHeader(QNetworkRequest::UserAgentHeader, user_agent);
}
