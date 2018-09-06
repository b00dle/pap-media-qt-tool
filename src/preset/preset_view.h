#ifndef PRESET_PRESET_VIEW_H
#define PRESET_PRESET_VIEW_H

#include <QListView>
#include <QPoint>
#include <QMouseEvent>
#include <QMenu>

#include "misc/standard_item_model.h"
#include "db/model/preset_table_model.h"

namespace Preset {

class PresetView : public QListView
{
    Q_OBJECT
public:
    explicit PresetView(QWidget *parent = 0);
    ~PresetView();

    void setPresetTableModel(DB::Model::PresetTableModel* model);
    DB::Model::PresetTableModel* getPresetTableModel();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

signals:

public slots:

protected slots:
    void showCustomContextMenu(const QPoint&);
    void onDeleteAction();

protected:
    void performDrag();
    void initContextMenu();

    QPoint start_pos_;
    DB::Model::PresetTableModel* model_;
    QMenu* context_menu_;
};

} // namespace Preset

#endif // PRESET_PRESET_VIEW_H
