#ifndef SOUND_FILE_PLAYER_H
#define SOUND_FILE_PLAYER_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QMediaPlayer>
#include <QSlider>

#include "db/table_records.h"

class SoundFilePlayer : public QWidget
{
    Q_OBJECT
public:
    explicit SoundFilePlayer(QWidget *parent = nullptr);
    virtual ~SoundFilePlayer();

    void setSoundFile(const DB::SoundFileRecord& sf, bool auto_play=false);

signals:

public slots:
    void play();
    void pause();
    void stop();

protected slots:
    void onPlayButtonClicked();
    void onPlaybackPositionChanged(qint64 v);
    void onPlaybackDurationChanged(qint64 v);
    void onProgressChanged(int v);

protected:
    void resetPlayer();

    void initWidgets();
    void initLayout();

    static QString const timeString(qint64 milliseconds);

    QMediaPlayer* player_;
    QMediaPlaylist* playlist_;
    QPushButton* play_button_;
    QLabel* current_sound_;
    QSlider* progress_;
    QLabel* current_time_;
    QLabel* total_time_;
    bool is_playing_;
    QIcon play_icon_;
    QIcon pause_icon_;
    bool ignore_progress_change_;
};

#endif // SOUND_FILE_PLAYER_H
