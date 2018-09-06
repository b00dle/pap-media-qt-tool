#include "spotify_tile.h"
#include "resources/lib.h"

#include <QGraphicsScene>
#include <QDebug>
#include <QMenu>
#include <QJsonArray>
#include <QMessageBox>

#include "spotify/spotify_tile_configure_dialog.h"
#include "spotify/spotify_handler.h"
#include "spotify/spotify_remote_controller.h"

namespace Tile {

SpotifyTile::SpotifyTile(QGraphicsItem *parent)
    : BaseTile(parent)
    , is_playing_(false)
    , playback_info_()
    , settings_()
    , web_pixmap_()
    , background_pixmap_()
{
    setAcceptDrops(true);
    SpotifyHandler::instance()->addTile(this);
    connect(&web_pixmap_, &WebPixmap::contentChanged,
            this, &SpotifyTile::onWebImageChanged);
}

SpotifyTile::~SpotifyTile()
{
    SpotifyHandler::instance()->removeTile(this);
}


void SpotifyTile::paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    BaseTile::paint(painter, option, widget);

    QRectF p_rect(getPaintRect());
    if(p_rect.width() > 0 && p_rect.height() > 0) {
        if(background_pixmap_.isNull()) {
            painter->drawPixmap(
                (int) p_rect.x(),
                (int) p_rect.y(),
                (int) p_rect.width(),
                (int) p_rect.height(),
                *Resources::Lib::PX_SPUNGIFY
            );
        }
        else {
            painter->drawPixmap(
                (int) p_rect.x(),
                (int) p_rect.y(),
                (int) p_rect.width(),
                (int) p_rect.height(),
                background_pixmap_
            );
        }
        painter->drawPixmap(
            (int) p_rect.x(),
            (int) p_rect.y(),
            (int) p_rect.width(),
            (int) p_rect.height(),
            getPlayStatePixmap()
        );
    }
}

void SpotifyTile::receiveExternalData(const QMimeData *data)
{
    if(!data->text().contains("spotify"))
        return;
    if(data->text().startsWith("https://"))
        settings_.setFromWebLink(data->text());
    else
        settings_.setFromURI(data->text());
    onConfigure();
}


void SpotifyTile::receiveWheelEvent(QWheelEvent *event)
{
    // todo set to current volume
    int volume = 0;
    if (event->delta() < 0) {
        volume -= 3;
        if (volume < 0) {
            volume = 0;
        }
    }
    else if(event->delta() >= 0) {
        volume += 3;
        if (volume > 100){
            volume = 100;
        }
    }

    //    settings->volume = volume;
    //    if (pl->setSettings(settings)) {
    //        emit wheelChangedVolume(volume);
    //    }
}


const QJsonObject SpotifyTile::toJsonObject() const
{
    QJsonObject obj = BaseTile::toJsonObject();

    QJsonObject settings;
    settings["mode"] = settings_.mode;
    settings["playlist_uri"] = settings_.playlist_uri;
    settings["track_uri"] = settings_.track_uri;
    settings["repeat_mode"] = settings_.repeat_mode;
    settings["shuffle_enabled"] = settings_.shuffle_enabled;
    obj["settings"] = settings;

    return obj;
}

bool SpotifyTile::setFromJsonObject(const QJsonObject &obj)
{
    if(!BaseTile::setFromJsonObject(obj))
        return false;

    if(obj.contains("settings") && obj["settings"].isObject()) {
        QJsonObject settings(obj["settings"].toObject());
        settings_.mode = (SpotifyRemoteController::ResourceCategory) settings["mode"].toInt();
        settings_.playlist_uri = settings["playlist_uri"].toString();
        settings_.track_uri = settings["track_uri"].toString();
        settings_.repeat_mode = (SpotifyRemoteController::RepeatMode) settings["repeat_mode"].toInt();
        settings_.shuffle_enabled = settings["shuffle_enabled"].toBool();
        if(ensureAccessGranted()) {
            updatePlaybackInfo();
        }
        else {
            connect(&SpotifyHandler::instance()->remote, &SpotifyRemoteController::accessGranted,
                    this, &SpotifyTile::onAccessGrantedOnceUpdateInfo);
        }
    }

    return true;
}

const SpotifyRemoteController::Settings &SpotifyTile::getSettings() const
{
    return settings_;
}

/*void SpotifyTile::fromWebLink(const QString &link)
{
    QStringList link_components = link.split("/");

    if(link_components[3] == "track") {
        QString id = link_components.last().split("?")[0];
        QString uri = QString("spotify:track:%1").arg(id);

        settings_.mode = SpotifyRemoteController::Settings::Track;
        settings_.playlist_uri = "";
        settings_.track_uri = uri;
        settings_.shuffle_enabled = false;
        settings_.repeat_mode = SpotifyRemoteController::Off;
    } else {
        QString user_name = link_components[4];
        QString id = link_components.last().split("?")[0];
        QString uri = QString("spotify:user:%1:playlist:%2").arg(user_name).arg(id);

        settings_.mode = SpotifyRemoteController::Settings::Playlist;
        settings_.playlist_uri = uri;
        settings_.track_uri = "";
        settings_.shuffle_enabled = false;
        settings_.repeat_mode = SpotifyRemoteController::Off;
    }

    if(ensureAccessGranted()) {
        updatePlaybackInfo();
    }
    else {
        connect(&SpotifyHandler::instance()->remote, &SpotifyRemoteController::accessGranted,
                this, &SpotifyTile::onAccessGrantedOnceUpdateInfo);
    }
}*/

void SpotifyTile::setPlaying(bool playing)
{
    prepareGeometryChange();
    setIsPlaying(playing);
}

void SpotifyTile::play()
{
    if(is_playing_ || !ensureAccessGranted()) {
        connect(&SpotifyHandler::instance()->remote, &SpotifyRemoteController::accessGranted,
                this, &SpotifyTile::onAccessGrantedOncePlay);
        QMessageBox b;
        connect(this, &SpotifyTile::acceptAllNotifcations,
                &b, &QMessageBox::accept);
        b.setWindowTitle(tr("Requesting access to Spotify"));
        b.setText(tr("Companion is being connected to Spotify!"));
        b.setInformativeText(tr("You should see and follow the authentication prompts popping up in your browser shortly."));
        b.setStandardButtons(QMessageBox::Ok);
        b.setDefaultButton(QMessageBox::Ok);
        b.exec();
        return;
    }

    SpotifyHandler::instance()->play(this);
    setIsPlaying(true);
}

void SpotifyTile::stop()
{
    if(!is_playing_ || !ensureAccessGranted()) {
        connect(&SpotifyHandler::instance()->remote, &SpotifyRemoteController::accessGranted,
                this, &SpotifyTile::onAccessGrantedOnceStop);
        QMessageBox b;
        connect(this, &SpotifyTile::acceptAllNotifcations,
                &b, &QMessageBox::accept);
        b.setWindowTitle(tr("Requesting access to Spotify"));
        b.setText(tr("Companion is being connected to Spotify!"));
        b.setInformativeText(tr("You should see and follow the authentication prompts popping up in your browser shortly."));
        b.setStandardButtons(QMessageBox::Ok);
        b.setDefaultButton(QMessageBox::Ok);
        b.exec();
        return;
    }

    SpotifyHandler::instance()->stop();
    setIsPlaying(false);
}

void SpotifyTile::onActivate()
{
    if(is_playing_)
        stop();
    else
        play();

    BaseTile::onActivate();
}

void SpotifyTile::setVolume(int volume)
{
    //    player_->setVolume(volume);
    Q_UNUSED(volume);
    qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
    qDebug() << "  >" << "TODO: set volume";
}

int SpotifyTile::getVolume() const
{
    //    return player_->volume();

    qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
    qDebug() << "  >" << "TODO: get volume";

    return 0;
}

void SpotifyTile::onConfigure()
{
    if(!ensureAccessGranted()) {
        connect(&SpotifyHandler::instance()->remote, &SpotifyRemoteController::accessGranted,
                this, &SpotifyTile::onAccessGrantedOnceConfigure);
        QMessageBox b;
        connect(this, &SpotifyTile::acceptAllNotifcations,
                &b, &QMessageBox::accept);
        b.setWindowTitle(tr("Requesting access to Spotify"));
        b.setText(tr("Companion is being connected to Spotify!"));
        b.setInformativeText(tr("You should see and follow the authentication prompts popping up in your browser shortly."));
        b.setStandardButtons(QMessageBox::Ok);
        b.setDefaultButton(QMessageBox::Ok);
        b.exec();
        return;
    }

    SpotifyTileConfigureDialog d(settings_);

    if(d.exec() == SpotifyTileConfigureDialog::Rejected)
        return;

    settings_ = d.getSettings();
    updatePlaybackInfo();
}

void SpotifyTile::onAccessGrantedOnceConfigure()
{
    disconnect(&SpotifyHandler::instance()->remote, &SpotifyRemoteController::accessGranted,
               this, &SpotifyTile::onAccessGrantedOnceConfigure);
    emit acceptAllNotifcations();
    onConfigure();
}

void SpotifyTile::onAccessGrantedOncePlay()
{
    disconnect(&SpotifyHandler::instance()->remote, &SpotifyRemoteController::accessGranted,
               this, &SpotifyTile::onAccessGrantedOncePlay);
    emit acceptAllNotifcations();
    play();
}

void SpotifyTile::onAccessGrantedOnceStop()
{
    disconnect(&SpotifyHandler::instance()->remote, &SpotifyRemoteController::accessGranted,
               this, &SpotifyTile::onAccessGrantedOnceStop);
    emit acceptAllNotifcations();
    stop();
}

void SpotifyTile::onAccessGrantedOnceUpdateInfo()
{
    disconnect(&SpotifyHandler::instance()->remote, &SpotifyRemoteController::accessGranted,
               this, &SpotifyTile::onAccessGrantedOnceUpdateInfo);
    emit acceptAllNotifcations();
    updatePlaybackInfo();
}

void SpotifyTile::onWebImageChanged()
{
    prepareGeometryChange();
    background_pixmap_ = web_pixmap_.getPixmap();
}

void SpotifyTile::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    if(mode_ != MOVE && e->button() == Qt::LeftButton) {
        if(e->modifiers() & Qt::ControlModifier)
            return BaseTile::mouseReleaseEvent(e);
        if(is_playing_)
            stop();
        else
            play();
    }
    BaseTile::mouseReleaseEvent(e);
}


void SpotifyTile::createContextMenu()
{
    // create configure action
    QAction* configure_action = new QAction(tr("Configure..."),this);

    connect(configure_action, SIGNAL(triggered()),
            this, SLOT(onConfigure()));

    context_menu_->addAction(configure_action);
    context_menu_->addSeparator();

    BaseTile::createContextMenu();
}

const QPixmap SpotifyTile::getPlayStatePixmap() const
{
    if(is_playing_)
        return *Resources::Lib::PX_STOP;
    else
        return *Resources::Lib::PX_PLAY;
}

void SpotifyTile::setIsPlaying(bool state)
{
    //is_activated_ = state;
    is_playing_ = state;
}

bool SpotifyTile::ensureAccessGranted()
{
    if(SpotifyHandler::isAccessGranted())
        return true;
    SpotifyHandler::requestGrantAccess();
    return false;
}

void SpotifyTile::updatePlaybackInfo()
{
    QNetworkReply *reply = nullptr;
    if(settings_.mode == SpotifyRemoteController::RC_Playlist) {
        reply = SpotifyHandler::instance()->playlistInfo(settings_.playlist_uri);
    }
    else if(settings_.mode == SpotifyRemoteController::RC_Track) {
        QStringList track_uri_components = settings_.track_uri.split(":");
        reply = SpotifyHandler::instance()->trackInfo(track_uri_components.last());
    }
    // connect to finished event of reply to get the servers response
    connect(reply, &QNetworkReply::finished,
            this, [=]() {
        playback_info_ = QJsonDocument::fromJson(reply->readAll());
        reply->deleteLater();
        if(!playback_info_.isObject())
            return;
        if(!playback_info_.object().contains("name"))
            return;
        prepareGeometryChange();
        setName(playback_info_.object()["name"].toString());
        updateRemotePixmap();
    });
}

void SpotifyTile::updateRemotePixmap()
{
    if(!playback_info_.isObject())
        return;
    QUrl url(getImageUrl(playback_info_.object()));
    if(!url.isEmpty())
        web_pixmap_.setUrl(url);
}

const QUrl SpotifyTile::getImageUrl(const QJsonObject& info)
{
    QJsonArray img_array;
    if(info.contains("images") && info["images"].isArray()) {
        img_array = info["images"].toArray();
    }
    else if(info.contains("album") && info["album"].isObject()) {
        QJsonObject album = info["album"].toObject();
        if(album.contains("images") && album["images"].isArray())
            img_array = album["images"].toArray();
    }

    if(img_array.size() == 0 || !img_array[0].toObject().contains("url"))
        return QUrl();

    return QUrl(img_array[0].toObject()["url"].toString());
}

} // namespace Tile
