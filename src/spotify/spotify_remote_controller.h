#ifndef SPOTIFY_REMOTE_HANDLER_H
#define SPOTIFY_REMOTE_HANDLER_H

#include <QObject>
#include <QQueue>
#include <QJsonDocument>

#include "oauth2_request_handler.h"

class SpotifyRemoteController : public QObject
{
    Q_OBJECT
public:
    enum RepeatMode {RM_Track, RM_Context, RM_Off};
    enum ResourceCategory {RC_Playlist, RC_Track};

    struct Settings
    {
        Settings()
            : mode(RC_Playlist)
            , track_uri()
            , playlist_uri()
            , repeat_mode(RM_Context)
            , shuffle_enabled(false)
        {}

        ResourceCategory mode;
        QString track_uri;
        QString playlist_uri;
        RepeatMode repeat_mode;
        bool shuffle_enabled;

        void setFromWebLink(const QString& link)
        {
            QStringList link_components = link.split("/");

            if(link_components[3] == "track") {
                QString id = link_components.last().split("?")[0];
                QString uri = QString("spotify:track:%1").arg(id);

                mode = RC_Track;
                playlist_uri = "";
                track_uri = uri;
            } else {
                QString user_name = link_components[4];
                QString id = link_components.last().split("?")[0];
                QString uri = QString("spotify:user:%1:playlist:%2").arg(user_name).arg(id);

                mode = RC_Playlist;
                playlist_uri = uri;
                track_uri = "";
            }
        }

        void setFromURI(const QString& uri) {
            if(uri.contains(":track:")) {
                mode = RC_Track;
                playlist_uri = "";
                track_uri = uri;
            }
            else if(uri.contains(":playlist:")) {
                mode = RC_Playlist;
                playlist_uri = uri;
                track_uri = "";
            }
            else {
                playlist_uri = "";
                track_uri = "";
            }
        }
    };

    explicit SpotifyRemoteController(QObject *parent = nullptr);
    virtual ~SpotifyRemoteController();

    void grantAccess();

    QNetworkReply *play();
    QNetworkReply *playUserPlaylist(const QString &user, const QString &playlist_id);
    QNetworkReply *playUserPlaylist(const QString &spotify_uri);
    QNetworkReply *playTrack(const QString &spotify_uri);

    QNetworkReply *pause();
    QNetworkReply *next();
    QNetworkReply *prev();
    QNetworkReply *setRepeat(RepeatMode mode);
    QNetworkReply *setShuffle(bool enable);
    QNetworkReply *setVolume(int value);

    QNetworkReply *getPlaylistInfo(const QString &user, const QString &playlist_id);
    QNetworkReply *getPlaylistInfo(const QString &spotify_uri);

    QNetworkReply *getPlaylistTracks(const QString &user, const QString &playlist_id);
    QNetworkReply *getPlaylistTracks(const QString &spotify_uri);

    QNetworkReply *getTrackInfo(const QString &track_id);

    bool isAccessGranted() const;

signals:
    void accessGranted();
    void authorizeWithBrowser(const QUrl &url);

public slots:
    void onAccessGranted();
    void onTokenChanged(const QString &token);

private:
    OAuth2RequestHandler *request_handler_;
    SpotifyAuthenticator *authenticator_;
    bool access_granted_;
};


#endif // SPOTIFY_REMOTE_HANDLER_H
