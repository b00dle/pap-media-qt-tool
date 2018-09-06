#include "spotify_authenticator.h"

#include <QDesktopServices>
#include <QOAuthHttpServerReplyHandler>

#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDesktopServices>

#include <QMessageBox>

SpotifyAuthenticator::SpotifyAuthenticator(QObject *parent)
    : QObject(parent)
    , client_id_()
    , client_secret_()
    , spotify_()
    , token_()
    , initialized_(false)
{}

SpotifyAuthenticator::~SpotifyAuthenticator()
{}

void SpotifyAuthenticator::loadCredentials()
{
    QString value;

    QString filepath = ":/config/spotify_credentials.json";
    QFile file;
    file.setFileName(filepath);

    if(file.open(QIODevice::ReadOnly)) {
        value = file.readAll();
        file.close();
    } else {
        file.close();
        QMessageBox b;
        b.setText("Spotify Credentials file missing!");
        b.exec();
    }

    QJsonParseError *error = new QJsonParseError;
    QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(), error);

    qDebug() << "    > "  << "Parsing Spotify credentials";
    qDebug() << "      > from" << filepath;
    qDebug() << "      > status" << error->errorString();

    QJsonObject obj = document.object();
    client_id_ = obj["client_id"].toString();
    client_secret_ = obj["client_secret"].toString();
}


QString SpotifyAuthenticator::getToken() const {
    return token_;
}

void SpotifyAuthenticator::grant() {
    if(!initialized_)
        init();
    spotify_.grant();
}

void SpotifyAuthenticator::onAccessGranted() {
    token_ = spotify_.token();
    emit accessGranted();
}

void SpotifyAuthenticator::init()
{
    loadCredentials();

    auto replyHandler = new QOAuthHttpServerReplyHandler(8080, this);
    spotify_.setReplyHandler(replyHandler);
    spotify_.setAuthorizationUrl(QUrl("https://accounts.spotify.com/authorize"));
    spotify_.setAccessTokenUrl(QUrl("https://accounts.spotify.com/api/token"));

    spotify_.setClientIdentifier(client_id_);
    spotify_.setClientIdentifierSharedKey(client_secret_);

    spotify_.setScope("user-modify-playback-state user-read-currently-playing user-read-playback-state user-read-birthdate user-read-email user-read-private user-follow-read user-follow-modify streaming user-library-modify user-library-read playlist-read-collaborative playlist-read-private playlist-modify-private playlist-modify-public user-read-recently-played user-top-read");

    // connections
    connect(&spotify_, &QOAuth2AuthorizationCodeFlow::granted,
            this, &SpotifyAuthenticator::onAccessGranted);

    connect(&spotify_, &QOAuth2AuthorizationCodeFlow::tokenChanged,
            this, &SpotifyAuthenticator::tokenChanged);

    connect(&spotify_, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
            &QDesktopServices::openUrl);

    initialized_ = true;
}
