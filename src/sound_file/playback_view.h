#ifndef PLAYBACK_VIEW_H
#define PLAYBACK_VIEW_H

#include <QTableView>

#include <QPoint>
#include <QMouseEvent>
#include <QStandardItemModel>
#include <QPushButton>
#include <QMenu>

#include "db/table_records.h"
#include "misc/standard_item_model.h"

class PlaybackView : public QTableView
{
    Q_OBJECT
public:
    explicit PlaybackView(QList<DB::SoundFileRecord*> const& sound_files, QWidget *parent = nullptr);
    explicit PlaybackView(QWidget *parent = nullptr);
    virtual ~PlaybackView();

    void setSoundFiles(QList<DB::SoundFileRecord*> const&);
    void setEditable(bool);
    bool getEditable();

protected:
    QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex &index, const QEvent *event = 0) const;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

protected:
    virtual void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);

protected slots:
    void onPlayButtonClicked();

signals:
    void play(const DB::SoundFileRecord&);
    void deleteSoundFileRequested(int id);

public slots:
    void addSoundFile(DB::SoundFileRecord* rec);
    void onSoundFileAboutToBeDeleted(DB::SoundFileRecord* rec);
    void onDropSuccessful();

private slots:
    void addSoundFile(int id, QString const& name, QString const& path);
    void onEntered(const QModelIndex&);
    void showCustomContextMenu(const QPoint&);
    void onDeleteAction();

protected:
    void performDrag();
    void initContextMenu();
    void init();

    QPoint start_pos_;
    Misc::StandardItemModel* model_;
    bool skip_select_;
    QPersistentModelIndex playable_index_;
    QIcon play_icon_;
    QMenu* context_menu_;
};

#endif // PLAYBACK_VIEW_H
