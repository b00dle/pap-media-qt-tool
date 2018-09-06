#ifndef SPOTIFY_TILE_H
#define SPOTIFY_TILE_H

#include <QMediaPlayer>
#include <QGraphicsView>
#include <QMimeData>
#include <QDrag>

#include "base_tile.h"
#include "misc/json_mime_data_parser.h"
#include "spotify/spotify_remote_controller.h"
#include "resources/web_pixmap.h"

namespace Tile {

class SpotifyTile : public BaseTile
{
    Q_OBJECT

public:
    SpotifyTile(QGraphicsItem* parent = 0);
    virtual ~SpotifyTile();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    virtual void receiveExternalData(const QMimeData* data);
    virtual void receiveWheelEvent(QWheelEvent *event);

    /**
     * Returns a QJsonObject holding all information about the tile
    */
    virtual const QJsonObject toJsonObject() const;

    /**
     * Set all values held by JSON object.
     * Returns success of parsing JsonObject.
    */
    virtual bool setFromJsonObject(const QJsonObject& obj);

    /**
     * @brief Returns the current settings object
     * @return
     */
    const SpotifyRemoteController::Settings &getSettings() const;

    /**
     * @brief creates Tile from WebLink
     * @param link
    void fromWebLink(const QString &link);
    */

    /**
     * @brief set Playing state
     * @param playing
     */
    void setPlaying(bool playing);

public slots:
    virtual void play();
    virtual void stop();
    virtual void onActivate();
    void setVolume(int volume);
    int getVolume() const;

signals:
    void wheelChangedVolume(int val);
    void acceptAllNotifcations();

protected slots:
    /** slot to open configuration */
    virtual void onConfigure();

    /**
     * connects to accessGranted of SpotifyHandler remote.
     * Calls onConfigure(). Disconnects as soon as executed once.
    */
    void onAccessGrantedOnceConfigure();

    /**
     * connects to accessGranted of SpotifyHandler remote.
     * Calls play(). Disconnects as soon as executed once.
    */
    void onAccessGrantedOncePlay();

    /**
     * connects to accessGranted of SpotifyHandler remote.
     * Calls stop(). Disconnects as soon as executed once.
    */
    void onAccessGrantedOnceStop();

    /**
     * connects to accessGranted of SpotifyHandler remote.
     * Calls updatePlaylistInfo(). Disconnects as soon as executed once.
    */
    void onAccessGrantedOnceUpdateInfo();

    void onWebImageChanged();

protected:
    /*
     * BC overrides
    */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);

    /**
     * creates context menu
    */
    virtual void createContextMenu();

    /**
     * Returns the image based on the playback state
    */
    virtual const QPixmap getPlayStatePixmap() const;

    void setIsPlaying(bool state);

    /**
     * Ensures the access to the spotify api has been granted.
     * Attempts to grant access if not granted yet.
     * Returns true if access granted, false otherwise.
    */
    bool ensureAccessGranted();

    /**
     * Updates the playback info of this tile,
     * Will trigger an update of the tile name
     * as soon as data has been received.
    */
    void updatePlaybackInfo();

    /**
     * Attempt to update the image linked to the playback resource.
    */
    void updateRemotePixmap();

    static const QUrl getImageUrl(const QJsonObject&);

    bool is_playing_;
    QJsonDocument playback_info_;
    SpotifyRemoteController::Settings settings_;
    WebPixmap web_pixmap_;
    QPixmap background_pixmap_;
};

} // namespace Tile

#endif // SPOTIFY_TILE_H

