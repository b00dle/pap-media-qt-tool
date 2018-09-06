#ifndef SPOTIFY_HANDLER_H
#define SPOTIFY_HANDLER_H

#include <QList>
#include <QObject>

#include "spotify_remote_controller.h"
#include "tile/spotify_tile.h"

#include <QNetworkReply>

// singleton implementation taken from
// http://www.yolinux.com/TUTORIALS/C++Singleton.html

class SpotifyHandler : public QObject
{
    Q_OBJECT
public:
    static SpotifyHandler* instance();
    virtual ~SpotifyHandler();

    // delete copy and move c'tors
    SpotifyHandler(const SpotifyHandler &) = delete;
    SpotifyHandler(SpotifyHandler &&) = delete;

    // delete assign operator
    void operator=(const SpotifyHandler&) = delete;
    void operator=(SpotifyHandler&&) = delete;

    void play(Tile::SpotifyTile *tile);
    void stop();

    void setVolume(int volume);

    void addTile(Tile::SpotifyTile *tile);
    void removeTile(Tile::SpotifyTile *tile);

    QNetworkReply *playlistInfo(const QString &uri);
    QNetworkReply *trackInfo(const QString &uri);

    SpotifyRemoteController remote;

    static bool isAccessGranted();
    static void requestGrantAccess();

private:
    explicit SpotifyHandler();

    Tile::SpotifyTile *active_tile_;
    QList<Tile::SpotifyTile*> existing_tiles_;
    static SpotifyHandler* instance_;
};

#endif // SPOTIFY_HANDLER_H
