#include "spotify_remote_controller.h"

#include "QDesktopServices"
#include <QJsonObject>
#include <QJsonDocument>
#include <QStringList>
#include <QJsonArray>

SpotifyRemoteController::SpotifyRemoteController(QObject *parent)
    : QObject(parent)
    , request_handler_(0)
    , authenticator_(0)
    , access_granted_(false)
{
    request_handler_ = new OAuth2RequestHandler(this);
    authenticator_ = new SpotifyAuthenticator(this);

    // authenticator connections
    connect(authenticator_, &SpotifyAuthenticator::accessGranted,
            this, &SpotifyRemoteController::onAccessGranted);

    connect(authenticator_, &SpotifyAuthenticator::tokenChanged,
            this, &SpotifyRemoteController::onTokenChanged);

    connect(authenticator_, &SpotifyAuthenticator::authorizeWithBrowser,
            this, &SpotifyRemoteController::authorizeWithBrowser);
}

SpotifyRemoteController::~SpotifyRemoteController()
{}

void SpotifyRemoteController::grantAccess() {
    authenticator_->grant();
}

void SpotifyRemoteController::onAccessGranted() {
    qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
    qDebug() << "  >" << "Setting token.";
    qDebug() << "SUCCESS: Spotify Access granted.";

    request_handler_->setRequestHeaders(authenticator_->getToken());
    access_granted_ = true;
    emit accessGranted();
}

void SpotifyRemoteController::onTokenChanged(const QString &token) {
    request_handler_->setRequestHeaders(token);
}

QNetworkReply *SpotifyRemoteController::play() {
    QString url = "me/player/play";
    return request_handler_->put(url);
}

QNetworkReply *SpotifyRemoteController::playUserPlaylist(const QString &user, const QString &playlist_id) {
    const QString url = "me/player/play";

    QJsonObject parameter_obj;
    parameter_obj["context_uri"] = QString("spotify:user:%1:playlist:%2").arg(user).arg(playlist_id);
    QJsonDocument parameter_doc(parameter_obj);

    return request_handler_->put(url, parameter_doc.toJson());
}

QNetworkReply *SpotifyRemoteController::playUserPlaylist(const QString &spotify_uri) {
    const QString url = "me/player/play";

    QJsonObject parameter_obj;
    parameter_obj["context_uri"] = spotify_uri;
    QJsonDocument parameter_doc(parameter_obj);

    return request_handler_->put(url, parameter_doc.toJson());
}

QNetworkReply *SpotifyRemoteController::playTrack(const QString &spotify_uri) {

    const QString url = "me/player/play";

    QJsonObject parameter_obj;
    QJsonArray tracks;
    tracks.append(spotify_uri);
    parameter_obj["uris"] = tracks;
    QJsonDocument parameter_doc(parameter_obj);

    return request_handler_->put(url, parameter_doc.toJson());
}

QNetworkReply *SpotifyRemoteController::pause() {
    const QString url = "me/player/pause";
    return request_handler_->put(url);
}

QNetworkReply *SpotifyRemoteController::next() {
    const QString url = "me/player/next";
    return request_handler_->post(url);
}

QNetworkReply *SpotifyRemoteController::prev() {
    const QString url = "me/player/previous";
    return request_handler_->post(url);
}

QNetworkReply *SpotifyRemoteController::setRepeat(RepeatMode mode) {

    QString url = "me/player/repeat";

    switch (mode) {
    case RM_Track:
        url += "?state=track";
        break;
    case RM_Context:
        url += "?state=context";
        break;
    case RM_Off:
        url += "?state=off";
        break;
    default:
        break;
    }

    return request_handler_->put(url);
}

QNetworkReply *SpotifyRemoteController::setShuffle(bool enabled) {
    QString enabled_str("true");
    if(!enabled)
        enabled_str = "false";
    QString url = QString("me/player/shuffle?state=%1").arg(enabled_str);
    return request_handler_->put(url);
}

QNetworkReply *SpotifyRemoteController::setVolume(int value) {
    QString url = QString("me/player/volume?volume_percent=%1").arg(value);
    return request_handler_->put(url);
}

QNetworkReply *SpotifyRemoteController::getPlaylistInfo(const QString &spotify_uri) {
    auto components = spotify_uri.split(":");

    QString url = QString("users/%1/playlists/%2").arg(components[2]).arg(components[4]);
    return request_handler_->get(url);
}

QNetworkReply *SpotifyRemoteController::getPlaylistInfo(const QString &user, const QString &playlist_id) {
    QString url = QString("users/%1/playlists/%2").arg(user).arg(playlist_id);
    return request_handler_->get(url);
}

QNetworkReply *SpotifyRemoteController::getPlaylistTracks(const QString &spotify_uri) {
    auto components = spotify_uri.split(":");
    QString url = QString("users/%1/playlists/%2/tracks").arg(components[2]).arg(components[4]);
    return request_handler_->get(url);
}

QNetworkReply *SpotifyRemoteController::getPlaylistTracks(const QString &user, const QString &playlist_id) {
    QString url = QString("users/%1/playlists/%2/tracks").arg(user).arg(playlist_id);
    return request_handler_->get(url);
}

QNetworkReply *SpotifyRemoteController::getTrackInfo(const QString &track_id) {
    QString url = QString("tracks/%1").arg(track_id);
    return request_handler_->get(url);
}

bool SpotifyRemoteController::isAccessGranted() const
{
    return access_granted_;
}
