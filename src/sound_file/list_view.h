#ifndef SOUND_FILE_LIST_VIEW_H
#define SOUND_FILE_LIST_VIEW_H

#include <QListView>

#include <QPoint>
#include <QMouseEvent>
#include <QStandardItemModel>

#include "db/table_records.h"
#include "misc/standard_item_model.h"

namespace SoundFile {

class ListView : public QListView
{
    Q_OBJECT
public:
    explicit ListView(QList<DB::SoundFileRecord*> const& sound_files, QWidget *parent = 0);
    explicit ListView(QWidget *parent = 0);
    virtual ~ListView();

    void setSoundFiles(QList<DB::SoundFileRecord*> const&);
    void setEditable(bool);
    bool getEditable();

protected:
    QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex &index, const QEvent *event = 0) const;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

protected:
    virtual void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);

public slots:
    void addSoundFile(DB::SoundFileRecord* rec);
    void onSoundFileAboutToBeDeleted(DB::SoundFileRecord* rec);
    void onDropSuccessful();

private slots:
    void addSoundFile(int id, QString const& name, QString const& path);

protected:
    void performDrag();

    QPoint start_pos_;
    Misc::StandardItemModel* model_;
    bool skip_select_;
    QModelIndex playable_index_;
};

} // namespace SoundFile

#endif // UI_SOUND_FILE_LIST_VIEW_H
