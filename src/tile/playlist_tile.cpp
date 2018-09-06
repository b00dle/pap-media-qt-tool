#include "playlist_tile.h"
#include "resources/lib.h"

#include <QGraphicsScene>
#include <QDebug>
#include <QMenu>
#include <QJsonArray>

#include "sound_file/list_view_dialog.h"
#include "misc/volume_mapper.h"

namespace Tile {

PlaylistTile::PlaylistTile(QGraphicsItem *parent)
    : BaseTile(parent)
    , player_(0)
    , playlist_settings_widget_(0)
    , playlist_(0)
    , model_(0)
    , is_playing_(false)
    , draw_filled_volume_indicator_(false)
    , filled_volume_timer_()
{
    player_ = new CustomMediaPlayer(this);
    //connect(player_, SIGNAL(stateChanged(QMediaPlayer::State)),
    //        this, SLOT(changePlayerState(QMediaPlayer::State)));

    connect(player_, SIGNAL(toggledPlayerActivation(bool)),
            this, SLOT(changedCustomPlayerActivation(bool)));

    filled_volume_timer_.setSingleShot(true);
    connect(&filled_volume_timer_, &QTimer::timeout,
            this, [=]() {
        prepareGeometryChange();
        draw_filled_volume_indicator_ = false;
    });

    playlist_ = new Playlist::MediaPlaylist("Playlist");
    player_->setPlaylist(playlist_);
    setAcceptDrops(true);
}

PlaylistTile::~PlaylistTile()
{
    playlist_->deleteLater();
    if(playlist_settings_widget_)
        playlist_settings_widget_->deleteLater();
}


void PlaylistTile::paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    BaseTile::paint(painter, option, widget);

    QRectF p_rect(getPaintRect());
    if(p_rect.width() > 0 && p_rect.height() > 0) {
        // draw current volume
        QRectF volume_rect(getVolumeRect());
        QLineF volume_line(volume_rect.topLeft(), volume_rect.topRight());
        qreal op = painter->opacity();
        painter->setOpacity(0.5);
        painter->setPen(QPen(QBrush(Qt::green), 5));
        painter->drawLine(volume_line);
        if(draw_filled_volume_indicator_)
            painter->fillRect(volume_rect, Qt::green);
        painter->setOpacity(op);

        // draw play state
        painter->drawPixmap(
            (int) p_rect.x(),
            (int)p_rect.y(),
            (int)p_rect.width(),
            (int)p_rect.height(),
            getPlayStatePixmap()
        );
    }

    /*
    int y= 0;
    //foreach(const QMediaContent* media,playlist_->media())
    for (int i = 0; i < playlist_->mediaCount(); i++)
    {
        QMediaContent c =playlist_->media(i);
        QUrl url = c.canonicalUrl();
        painter->drawText(QPointF(p_rect.x(), p_rect.y()+size_+y),url.fileName());
        y += 10;
    }
    */

}

void PlaylistTile::receiveExternalData(const QMimeData *data)
{
    // extract DB::TableRecord from mime data
    QList<DB::TableRecord*> records = Misc::JsonMimeDataParser::toTableRecordList(data);

    // validate parsing
    if(records.size() == 0)
        return;

    // add media for each sound file record
    foreach(DB::TableRecord* rec, records) {
        if(rec->index != DB::SOUND_FILE)
            continue;
        addMedia(*((DB::SoundFileRecord*) rec));
    }

    // delete temp records
    while(records.size() > 0) {
        delete records[0];
        records.pop_front();
    }
}


void PlaylistTile::receiveWheelEvent(QWheelEvent *event)
{
    Playlist::MediaPlaylist* pl = player_->getCustomPlaylist();
    Playlist::Settings settings = pl->getSettings();

    int log_volume = VolumeMapper::linearToLogarithmic(settings.volume);
    if (event->delta() < 0) {
        log_volume -= 3;
        if (log_volume < 0)
            log_volume = 0;
    }
    else if(event->delta() >= 0) {
        log_volume += 3;
        if (log_volume > 100)
            log_volume = 100;
    }

    int new_volume = VolumeMapper::logarithmicToLinear(log_volume);
    if(new_volume != settings.volume) {
        setVolume(new_volume);
        emit wheelChangedVolume(settings.volume);
    }
}

bool PlaylistTile::addMedia(int record_id)
{
    if(model_ == 0)
        return false;

    return playlist_->addMedia(record_id);
}

bool PlaylistTile::addMedia(const DB::SoundFileRecord &r)
{
    if(model_ == 0)
        return false;

    return playlist_->addMedia(r);
}

void PlaylistTile::setSoundFileModel(DB::Model::SoundFileTableModel *m)
{
    model_ = m;
    playlist_->setSoundFileModel(model_);
}

DB::Model::SoundFileTableModel *PlaylistTile::getSoundFileModel()
{
    return model_;
}

const QJsonObject PlaylistTile::toJsonObject() const
{
    QJsonObject obj = BaseTile::toJsonObject();

    // store playlist
    QJsonArray arr_pl;
    foreach(DB::SoundFileRecord* rec, playlist_->getSoundFileList())
        arr_pl.append(Misc::JsonMimeDataParser::toJsonObject(rec));
    obj["playlist"] = arr_pl;

    //store settings
    QJsonObject obj_settings;
    obj_settings = Misc::JsonMimeDataParser::toJsonObject(playlist_->getSettings());
    obj["settings"] = obj_settings;

    return obj;
}

bool PlaylistTile::setFromJsonObject(const QJsonObject &obj)
{
    if(!BaseTile::setFromJsonObject(obj))
        return false;

    // parse playlist
    if(obj.contains("playlist") && obj["playlist"].isArray()) {
        foreach(QJsonValue val, obj["playlist"].toArray()) {
            QJsonObject sound_obj = val.toObject();
            if(sound_obj.isEmpty())
                continue;

            DB::TableRecord* rec = Misc::JsonMimeDataParser::toTableRecord(sound_obj);
            if(rec->index != DB::SOUND_FILE) {
                delete rec;
                continue;
            }

            // check existance against actual database
            DB::SoundFileRecord* sf_rec = (DB::SoundFileRecord*) rec;
            QList<DB::SoundFileRecord*> actual_recs = model_->getSoundFilesByRelativePath(sf_rec->relative_path);
            if(actual_recs.size() == 0) {
                qDebug() << "FAILURE: Could not verify SoundFile existance.";
                qDebug() << " > SoundFile:" << sound_obj << "does not exist in any ResourceDirectory.";
                qDebug() << " > Make sure relative path (" << sf_rec->relative_path << ") exists within a ResourceDirectory.";
                sf_rec = 0;
                delete rec;
                return false;
            }
            else if(actual_recs.size() > 1) {
                bool found = false;
                foreach(DB::SoundFileRecord* act_rec, actual_recs) {
                    if(act_rec->id == sf_rec->id) {
                        sf_rec->copyFrom(act_rec);
                        found = true;
                        break;
                    }
                }

                if(!found) {
                    qDebug() << "NOTIFICATION: More than one SoundFile exists for relative path";
                    qDebug() << " > And ID of SoundFiles parsed from JSON cannot be found in database.";
                    qDebug() << " > automatically picking first matched SoundFile.";
                    sf_rec->copyFrom(actual_recs[0]);
                }


            }
            else { // exactly one SoundFIle matches
                sf_rec->copyFrom(actual_recs[0]);
            }

            bool success = playlist_->addMedia(*sf_rec);
            if(!success) {
                qDebug() << "FAILURE: Could not add SoundFile from JSON";
                qDebug() << " > " << sound_obj;
                sf_rec = 0;
                delete rec;
                return false;
            }

            sf_rec = 0;
            delete rec;
        }
    }

    // parse settings
    if(obj.contains("settings") && obj["settings"].isObject() ) {
        QJsonObject s_obj = obj["settings"].toObject();
        if(s_obj.isEmpty())
            return false;
        //to do: no pointer needed
        Playlist::Settings* settings = Misc::JsonMimeDataParser::toPlaylistSettings(s_obj);
        if(!settings) {
            qDebug() << "FAILURE: Could not set Playlist Settings from JSON";
            qDebug() << " > " << s_obj;
        }
        playlist_->setSettings(*settings);
        delete settings;
        settings = 0;
    }

    return true;
}

void PlaylistTile::setMedia(const QMediaContent &c)
{
    player_->setMedia(c);
}

void PlaylistTile::play()
{
    if(!player_->media().isNull() && !is_playing_) {
        player_->activate();
        player_->play();
        setIsPlaying(true);
    }
}

void PlaylistTile::stop()
{
    if(!player_->media().isNull() && is_playing_) {
        player_->stop();
        player_->deactivate();
        setIsPlaying(false);
    }
}

void PlaylistTile::onActivate()
{
    if(is_playing_)
        stop();
    else
        play();

    BaseTile::onActivate();
}

void PlaylistTile::setVolume(int volume)
{
    Playlist::Settings settings = player_->getCustomPlaylist()->getSettings();
    settings.volume = volume;
    player_->getCustomPlaylist()->setSettings(settings);
    volumeChangedEvent();
    //player_->setVolume(volume);
}

int PlaylistTile::getVolume() const
{
    return player_->volume();
}

void PlaylistTile::changePlayerState(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::PlayingState){
        setIsPlaying(true);
    } else if (state == QMediaPlayer::StoppedState){
        setIsPlaying(false);
    }
}

void PlaylistTile::changedCustomPlayerActivation(bool state)
{
    if (state == true){
        setIsPlaying(true);
    } else if (state == false){
        setIsPlaying(false);
    }
}

void PlaylistTile::onConfigurePlaylist()
{
    Playlist::Settings s = playlist_->getSettings();
    s.name = getName();
    if(!playlist_settings_widget_) {
        playlist_settings_widget_ = new Playlist::SettingsWidget(s);
        playlist_settings_widget_->setWindowTitle(tr("Playback Settings"));
        playlist_settings_widget_->move(QCursor::pos() - QPoint(170,170));
        connect(playlist_settings_widget_, &Playlist::SettingsWidget::closed,
                this, &PlaylistTile::closePlaylistSettings);
        connect(playlist_settings_widget_, &Playlist::SettingsWidget::saved,
                this, &PlaylistTile::savePlaylistSettings);
        connect(playlist_settings_widget_, &Playlist::SettingsWidget::volumeSettingsChanged,
                this, &PlaylistTile::setVolume);
        connect(this, &PlaylistTile::wheelChangedVolume,
                playlist_settings_widget_, &Playlist::SettingsWidget::onExternalVolumeChanged);
    }
    playlist_settings_widget_->popOpen();
}

void PlaylistTile::onContents()
{
    SoundFile::ListViewDialog d(playlist_->getSoundFileList());
    d.setAcceptDrops(false);

    if(d.exec()) {

    }
}

void PlaylistTile::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
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

void PlaylistTile::closePlaylistSettings()
{
    playlist_settings_widget_->deleteLater();
    playlist_settings_widget_ = 0;
}

void PlaylistTile::savePlaylistSettings(const Playlist::Settings &settings)
{
    if(settings.name.size() > 0 && name_.compare(settings.name) != 0)
        setName(settings.name);
    if(settings.image_path.size() > 0)
        loadOverlayPixmap(settings.image_path);
    else
        clearOverlayPixmap();

    playlist_->setSettings(settings);
    playlist_settings_widget_->hide();
    playlist_settings_widget_->deleteLater();
    playlist_settings_widget_ = nullptr;
}

void PlaylistTile::createContextMenu()
{
    // create configure action
    QAction* configure_action = new QAction(tr("Configure..."),this);

    connect(configure_action, SIGNAL(triggered()),
            this, SLOT(onConfigurePlaylist()));

    QAction* contents_action = new QAction(tr("Contents..."),this);

    connect(contents_action, SIGNAL(triggered()),
            this, SLOT(onContents()));

    context_menu_->addAction(configure_action);
    context_menu_->addAction(contents_action);
    context_menu_->addSeparator();

    BaseTile::createContextMenu();
}

const QPixmap PlaylistTile::getPlayStatePixmap() const
{
    if(is_playing_)
        return *Resources::Lib::PX_STOP;
    else
        return *Resources::Lib::PX_PLAY;
}

void PlaylistTile::volumeChangedEvent()
{
    prepareGeometryChange();
    draw_filled_volume_indicator_ = true;
    filled_volume_timer_.start(2500);
}

void PlaylistTile::setIsPlaying(bool state)
{
    is_playing_ = state;
}

const QRectF PlaylistTile::getVolumeRect() const
{
    int log_volume = player_->getCustomPlaylist()->getSettings().volume;
    log_volume = VolumeMapper::linearToLogarithmic(log_volume);
    QRectF volume_rect(getPaintRect());
    volume_rect.setTop(volume_rect.bottom()-volume_rect.height()*(log_volume/100.0f));
    return volume_rect;
}

} // namespace Tile
