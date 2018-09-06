#ifndef PLAYLIST_PLAYLIST_H
#define PLAYLIST_PLAYLIST_H

#include <QMediaPlaylist>
#include <QMap>
#include <QSet>

#include "settings.h"
#include "db/model/sound_file_table_model.h"

namespace Playlist {

class MediaPlaylist : public QMediaPlaylist
{
    Q_OBJECT
public:
    MediaPlaylist(QString name = "Playlist", QObject* parent = 0);
    virtual ~MediaPlaylist();

    void setSettings(const Settings& settings);
    const Settings& getSettings() const;

    /**
     * Set database for the playlist
     */
    void setSoundFileModel(DB::Model::SoundFileTableModel* m);
    const DB::Model::SoundFileTableModel* getSoundFileModel() const;

    bool addMedia(const DB::SoundFileRecord& rec);
    bool addMedia(int record_id);

    const QList<DB::SoundFileRecord*> getSoundFileList(bool unique = false);

signals:
    void changedSettings();

private slots:
    void onMediaAboutToBeRemoved(int start, int end);

private:
    QString name_;
    Settings* settings_;
    DB::Model::SoundFileTableModel* model_;
    QMap<QMediaContent*, DB::SoundFileRecord*> records_;

};

} // end namespace playlist

#endif // PLAYLIST_PLAYLIST_H
