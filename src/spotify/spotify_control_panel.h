#ifndef SPOTIFY_CONTROL_PANEL_H
#define SPOTIFY_CONTROL_PANEL_H

#include <QWidget>

#include <QPushButton>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSlider>
#include <QCheckBox>

#include <spotify/spotify_remote_controller.h>

class SpotifyControlPanel : public QWidget
{
    Q_OBJECT
public:
    explicit SpotifyControlPanel(QWidget *parent = nullptr);
    virtual ~SpotifyControlPanel();

signals:

public slots:
    void onAccessGranted();
    void onPlay();
    void onPause();
    void onNext();
    void onPrev();
    void onEnableShuffle();
    void onDisableShuffle();
    void onSetRepeatTrack();
    void onSetRepeatContext();
    void onSetRepeatOff();
    void onSetVolume(int value);

private:
    QPushButton* createButton(const QString &title, bool enabled, QWidget *parent);
    void initWidgets();
    void initLayout();
    void initSpotifyConnection();

private:
    QPushButton *btn_ctrl_play_;
    QPushButton *btn_ctrl_pause_;
    QPushButton *btn_ctrl_next_;
    QPushButton *btn_ctrl_prev_;
    QPushButton *btn_ctrl_enable_shuffle;
    QPushButton *btn_ctrl_disable_shuffle;
    QPushButton *btn_ctrl_set_repeat_track;
    QPushButton *btn_ctrl_set_repeat_context;
    QPushButton *btn_ctrl_set_repeat_off;
    QSlider *volume_slider_;
};

#endif // SPOTIFY_CONTROL_PANEL_H
