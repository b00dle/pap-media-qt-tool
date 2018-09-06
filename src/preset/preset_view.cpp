#include "preset_view.h"

#include <QDrag>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMimeData>
#include <QApplication>
#include <QDebug>

#include "resources/lib.h"

namespace Preset {

PresetView::PresetView(QWidget *parent)
    : QListView(parent)
    , start_pos_()
    , model_(0)
{
    initContextMenu();
    //setAcceptDrops(false);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setContextMenuPolicy(Qt::CustomContextMenu);
}

PresetView::~PresetView()
{}

void PresetView::setPresetTableModel(DB::Model::PresetTableModel *model)
{
    model_ = model;
    setModel(model_);
}

DB::Model::PresetTableModel *PresetView::getPresetTableModel()
{
    return model_;
}

void PresetView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        start_pos_ = event->pos();
    }
    QListView::mousePressEvent(event);
}

void PresetView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        int distance = (event->pos() - start_pos_).manhattanLength();
        if (distance >= QApplication::startDragDistance()) {
            performDrag();
        }
    }
    QListView::mouseMoveEvent(event);
}

void PresetView::dragEnterEvent(QDragEnterEvent *event)
{
    event->ignore();
}

void PresetView::dragMoveEvent(QDragMoveEvent *event)
{
    event->ignore();
}

void PresetView::dropEvent(QDropEvent *event)
{
    event->ignore();
}

void PresetView::showCustomContextMenu(const QPoint& p)
{
    context_menu_->exec(mapToGlobal(p));
}

void PresetView::onDeleteAction()
{
    QModelIndexList selection = this->selectionModel()->selectedIndexes();
    if(selection.size() == 0)
        return;

    int row = selection.first().row();
    model_->removeRow(row);
}

void PresetView::performDrag()
{
    if(selectionModel()->selectedRows().size() == 1) {
        if (model_ == 0)
            return;

        QModelIndex idx = selectionModel()->selectedRows(1)[0];

        DB::PresetRecord* rec = model_->getPresetByRow(idx.row());
        QMimeData* mime_data = new QMimeData;
        mime_data->setText(rec->json);

        // create Drag
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mime_data);
        drag->setPixmap(*Resources::Lib::PX_SOUND_FILE_DRAG);

        // will block until drag done
        drag->exec(Qt::CopyAction);
    }
}

void PresetView::initContextMenu()
{
    context_menu_ = new QMenu(this);

    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(showCustomContextMenu(const QPoint&)));

    QList<QAction*> actions;
    actions.append(new QAction(tr("Delete"), context_menu_));
    connect(actions.back(), SIGNAL(triggered()),
            this, SLOT(onDeleteAction()));

    context_menu_->addActions(actions);
}

} // namespace Preset
