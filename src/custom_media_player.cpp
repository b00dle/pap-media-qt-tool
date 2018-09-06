#include "custom_media_player.h"

CustomMediaPlayer::CustomMediaPlayer(QObject* parent)
    : QMediaPlayer(parent)
    , activated_(false)
    , current_content_index_(0)
    , delay_flag_(false)
    , delay_(0)
    , delay_timer_(0)
{
    delay_timer_ = new QTimer(this);
    connect(delay_timer_, SIGNAL(timeout()),
            this, SLOT(delayIsOver()));

}

void CustomMediaPlayer::play()
{

    Playlist::MediaPlaylist* playlist = getCustomPlaylist();
    if(playlist) {
        Playlist::Settings settings = playlist->getSettings();
        setVolume(settings.volume);
        // if delay interval is turned on
        if (settings.order == Playlist::PlayOrder::ORDERED){
            if (settings.loop_flag){
                playlist->setPlaybackMode(QMediaPlaylist::Loop);

            }else if (!settings.loop_flag){
                playlist->setPlaybackMode(QMediaPlaylist::Sequential);

            }
        } else if (settings.order == Playlist::PlayOrder::SHUFFLE){
            playlist->setPlaybackMode(QMediaPlaylist::Random);
            int index = getRandomIntInRange(0, playlist->mediaCount()-1);
            playlist->setCurrentIndex(index);

        } else if (settings.order == Playlist::PlayOrder::WEIGHTED){
            // TODO implement weighted
            QMediaPlayer::pause();
        }

        if (settings.interval_flag){
            delay_ = getRandomIntInRange(settings.min_delay_interval,
                                         settings.max_delay_interval);
            delay_flag_ = true;
            if (activated_){
                QMediaPlayer::play();
                playlist->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
            }
        } else {
            delay_flag_ = false;
            delay_ = 0;
            if (activated_){
                QMediaPlayer::play();
            }
        }
    }
}

void CustomMediaPlayer::setPlaylist(Playlist::MediaPlaylist *playlist)
{
    connect(playlist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(currentMediaIndexChanged(int)) );

    connect(playlist, SIGNAL(changedSettings()),
            this, SLOT(mediaSettingsChanged()) );

    QMediaPlayer::setPlaylist(playlist);
}

void CustomMediaPlayer::delayIsOver()
{
    delay_timer_->stop();
    play();
}

void CustomMediaPlayer::activate()
{
    activated_ = true;
    emit toggledPlayerActivation(true);
}

void CustomMediaPlayer::deactivate()
{
    activated_ = false;
    emit toggledPlayerActivation(false);
}

void CustomMediaPlayer::setActivation(bool flag)
{
    activated_ = flag;
    emit toggledPlayerActivation(flag);
}

int CustomMediaPlayer::getRandomIntInRange(int min, int max)
{
    int range = max - min;
    return (qrand() % (range+1)) + min;
}

void CustomMediaPlayer::currentMediaIndexChanged(int position)
{
    current_content_index_ = position;
    if (activated_ && delay_flag_){
        delay_timer_->start(delay_*1000);
    }
}

void CustomMediaPlayer::mediaSettingsChanged()
{
    Playlist::Settings settings = getCustomPlaylist()->getSettings();
    setVolume(settings.volume);
    if (settings.interval_flag){
        delay_flag_ = true;
        delay_ = getRandomIntInRange(settings.min_delay_interval,
                                     settings.max_delay_interval);
    } else {
        delay_flag_ = false;
        delay_ = 0;
    }

    if (settings.order == Playlist::PlayOrder::ORDERED){

    } else if (settings.order == Playlist::PlayOrder::SHUFFLE){
        getCustomPlaylist()->setPlaybackMode(QMediaPlaylist::Random);
    } else if (settings.order == Playlist::PlayOrder::WEIGHTED){
        qDebug() << "weigthed not implemented yet";
    }
}

void CustomMediaPlayer::mediaVolumeChanged(int val)
{
    if (val >= 0 && val <= 100)
        setVolume(val);
}


Playlist::MediaPlaylist *CustomMediaPlayer::getCustomPlaylist() const
{

    Playlist::MediaPlaylist* pl = qobject_cast<Playlist::MediaPlaylist*>( playlist() );
    if(pl){
        return pl;
    } else {
        return nullptr;
    }
}

