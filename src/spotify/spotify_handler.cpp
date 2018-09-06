#include "spotify_handler.h"

#include <QMessageBox>

SpotifyHandler* SpotifyHandler::instance_ = nullptr;

SpotifyHandler::SpotifyHandler()
    : QObject()
    , remote()
    , active_tile_(0)
    , existing_tiles_()
{}

SpotifyHandler* SpotifyHandler::instance() {
    if(!instance_) {
        instance_ = new SpotifyHandler;
    }
    return instance_;
}

SpotifyHandler::~SpotifyHandler() {
    instance_->deleteLater();
}

void SpotifyHandler::play(Tile::SpotifyTile *selected_tile)
{
    SpotifyRemoteController::Settings current_settings = selected_tile->getSettings();
    QNetworkReply* play_reply = nullptr;
    switch (current_settings.mode) {
        case SpotifyRemoteController::RC_Playlist:
            play_reply = remote.playUserPlaylist(current_settings.playlist_uri);
            break;
        case SpotifyRemoteController::RC_Track:
            play_reply = remote.playTrack(current_settings.track_uri);
            break;
        default:
            qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
            qDebug() << "  >" << "Active Tile exists!";
            qDebug() << "    >" << "Error in getting settings mode!";
            break;
    }

    if(!play_reply)
        return;

    active_tile_ = selected_tile;

    // after play request done, do shuffle
    connect(play_reply, &QNetworkReply::finished,
            this, [=]() {
        QNetworkReply* shuffle_reply = remote.setShuffle(current_settings.shuffle_enabled);
        // after shuffle request done, do repeat
        connect(shuffle_reply, &QNetworkReply::finished,
                this, [=]() {
            remote.setRepeat(current_settings.repeat_mode);
            shuffle_reply->deleteLater();
        });
        play_reply->deleteLater();
    });

    // disable all other tiles
    for(auto tile : existing_tiles_) {
        if(tile != selected_tile) {
            tile->setPlaying(false);
        }
    }

}


void SpotifyHandler::stop()
{
    remote.pause();
    for(auto tile: existing_tiles_) {
        tile->setPlaying(false);
    }
}

void SpotifyHandler::setVolume(int volume)
{
    remote.setVolume(volume);
}

void SpotifyHandler::addTile(Tile::SpotifyTile *tile)
{
    existing_tiles_.append(tile);
    /*
    qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
    qDebug() << "  >" << "added tile to active tile list";
    qDebug() << "    >" << "number of existing spotify tiles:" << existing_tiles_.length();
    */
}

void SpotifyHandler::removeTile(Tile::SpotifyTile *tile)
{
    existing_tiles_.removeAll(tile);
}

QNetworkReply *SpotifyHandler::playlistInfo(const QString &uri)
{
    return remote.getPlaylistInfo(uri);
}

QNetworkReply *SpotifyHandler::trackInfo(const QString &track_id)
{
    return remote.getTrackInfo(track_id);
}

bool SpotifyHandler::isAccessGranted()
{
    return instance_ != 0 && instance_->remote.isAccessGranted();
}

void SpotifyHandler::requestGrantAccess()
{
    if(!isAccessGranted()) {
        if (!QSslSocket::supportsSsl()) {
            QMessageBox b;
            b.setText("Spotify authentication cannot be established.");
            b.setInformativeText("SSL not supported on this device.");
            b.exec();
            return;
        }

        qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
        qDebug() << "  >" << "initializing Spotify...";
        qDebug() << "     >" << "SSL version:" << QSslSocket::sslLibraryVersionString();

        instance()->remote.grantAccess();
    }
}
