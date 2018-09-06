#include "spotify_control_panel.h"

#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLabel>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QWidget>
#include <QSlider>

#include "spotify/spotify_handler.h"

SpotifyControlPanel::SpotifyControlPanel(QWidget *parent)
    : QWidget(parent)
    , btn_ctrl_play_(nullptr)
    , btn_ctrl_pause_(nullptr)
    , btn_ctrl_next_(nullptr)
    , btn_ctrl_prev_(nullptr)
    , btn_ctrl_enable_shuffle(nullptr)
    , btn_ctrl_disable_shuffle(nullptr)
    , btn_ctrl_set_repeat_track(nullptr)
    , btn_ctrl_set_repeat_context(nullptr)
    , btn_ctrl_set_repeat_off(nullptr)
    , volume_slider_(nullptr)
{
    initWidgets();
    initLayout();
    initSpotifyConnection();

    setWindowTitle("Spotify Control Panel");
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
}

SpotifyControlPanel::~SpotifyControlPanel()
{}

void SpotifyControlPanel::onAccessGranted()
{
    foreach(auto o, children()) {
        QWidget* w = qobject_cast<QWidget*>(o);
        if(w)
            w->setEnabled(true);
    }
}

void SpotifyControlPanel::onPlay()
{
    SpotifyHandler::instance()->remote.play();
}

void SpotifyControlPanel::onPause()
{
    SpotifyHandler::instance()->remote.pause();
}

void SpotifyControlPanel::onNext()
{
    SpotifyHandler::instance()->remote.next();
}

void SpotifyControlPanel::onPrev()
{
    SpotifyHandler::instance()->remote.prev();
}

void SpotifyControlPanel::onEnableShuffle()
{
    SpotifyHandler::instance()->remote.setShuffle(true);
}

void SpotifyControlPanel::onDisableShuffle()
{
    SpotifyHandler::instance()->remote.setShuffle(false);
}

void SpotifyControlPanel::onSetRepeatContext()
{
    SpotifyHandler::instance()->remote.setRepeat(SpotifyRemoteController::RepeatMode::RM_Context);
}

void SpotifyControlPanel::onSetRepeatTrack()
{
    SpotifyHandler::instance()->remote.setRepeat(SpotifyRemoteController::RepeatMode::RM_Track);
}

void SpotifyControlPanel::onSetRepeatOff()
{
    SpotifyHandler::instance()->remote.setRepeat(SpotifyRemoteController::RepeatMode::RM_Off);
}

void SpotifyControlPanel::onSetVolume(int value)
{
    SpotifyHandler::instance()->remote.setVolume(value);
}

QPushButton* SpotifyControlPanel::createButton(const QString &title, bool enabled, QWidget *parent)
{
    QPushButton *button = new QPushButton(title, parent);
    button->setEnabled(enabled);
    return button;
}

void SpotifyControlPanel::initWidgets()
{
    btn_ctrl_play_ = createButton("Play", false, this);
    btn_ctrl_pause_ = createButton("Pause", false, this);
    btn_ctrl_next_= createButton(">>", false, this);
    btn_ctrl_prev_= createButton("<<" , false, this);
    btn_ctrl_enable_shuffle= createButton("Enable Shuffle", false, this);
    btn_ctrl_disable_shuffle= createButton("Disable Shuffle", false, this);
    btn_ctrl_set_repeat_track= createButton("Repeat Track", false, this);
    btn_ctrl_set_repeat_context= createButton("Repeat Context", false, this);
    btn_ctrl_set_repeat_off= createButton("Repeat Off", false, this);
    volume_slider_ = new QSlider(Qt::Orientation::Horizontal, this);
    volume_slider_->setEnabled(false);

    connect(btn_ctrl_play_, &QPushButton::clicked,
            this, &SpotifyControlPanel::onPlay);
    connect(btn_ctrl_pause_, &QPushButton::clicked,
            this, &SpotifyControlPanel::onPause);
    connect(btn_ctrl_next_, &QPushButton::clicked,
            this, &SpotifyControlPanel::onNext);
    connect(btn_ctrl_prev_, &QPushButton::clicked,
            this, &SpotifyControlPanel::onPrev);
    connect(btn_ctrl_enable_shuffle, &QPushButton::clicked,
            this, &SpotifyControlPanel::onEnableShuffle);
    connect(btn_ctrl_disable_shuffle, &QPushButton::clicked,
            this, &SpotifyControlPanel::onDisableShuffle);
    connect(btn_ctrl_set_repeat_context, &QPushButton::clicked,
            this, &SpotifyControlPanel::onSetRepeatContext);
    connect(btn_ctrl_set_repeat_track, &QPushButton::clicked,
            this, &SpotifyControlPanel::onSetRepeatTrack);
    connect(btn_ctrl_set_repeat_off, &QPushButton::clicked,
            this, &SpotifyControlPanel::onSetRepeatOff);
    connect(volume_slider_, &QSlider::valueChanged,
            this, &SpotifyControlPanel::onSetVolume);
}

void SpotifyControlPanel::initLayout()
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(btn_ctrl_play_);
    layout->addWidget(btn_ctrl_pause_);
    layout->addWidget(btn_ctrl_next_);
    layout->addWidget(btn_ctrl_prev_);
    layout->addWidget(btn_ctrl_enable_shuffle);
    layout->addWidget(btn_ctrl_disable_shuffle);
    layout->addWidget(btn_ctrl_set_repeat_track);
    layout->addWidget(btn_ctrl_set_repeat_context);
    layout->addWidget(btn_ctrl_set_repeat_off);
    layout->addWidget(volume_slider_);
    setLayout(layout);
}

void SpotifyControlPanel::initSpotifyConnection()
{
    connect(&SpotifyHandler::instance()->remote, &SpotifyRemoteController::accessGranted,
            this, &SpotifyControlPanel::onAccessGranted);

    // grant access to spotify
    if(!SpotifyHandler::isAccessGranted())
        SpotifyHandler::requestGrantAccess();
    else
        onAccessGranted();
}
