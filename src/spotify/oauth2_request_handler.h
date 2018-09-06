#ifndef OAUTH2_REQUEST_HANDLER_H
#define OAUTH2_REQUEST_HANDLER_H

#include <QObject>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QVariantMap>

#include "spotify_authenticator.h"

class OAuth2RequestHandler : public QObject
{
    Q_OBJECT
public:
    explicit OAuth2RequestHandler(QObject *parent = nullptr);
    virtual ~OAuth2RequestHandler();

    QNetworkReply *get(const QString &url);

    QNetworkReply *put(const QString &url, const QByteArray &parameter);
    QNetworkReply *put(const QString &url);

    QNetworkReply *post(const QString &url, const QByteArray &parameter);
    QNetworkReply *post(const QString &url);

signals:
    void requestFinished(QNetworkReply *reply);

public slots:
    void setRequestHeaders(const QString &token);

private:
    QString base_url = "https://api.spotify.com/v1/";

    QNetworkAccessManager * network_manager_;
    QNetworkRequest request_;
    bool access_granted_;

};

#endif // OAUTH2_REQUEST_HANDLER_H
