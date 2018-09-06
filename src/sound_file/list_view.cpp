#include "list_view.h"

#include <QDebug>
#include <QMimeData>
#include <QApplication>
#include <QDrag>
#include <QPixmap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPushButton>

#include "resources/lib.h"
#include "misc/json_mime_data_parser.h"

namespace SoundFile {

ListView::ListView(QList<DB::SoundFileRecord*> const& sound_files, QWidget *parent)
    : QListView(parent)
    , start_pos_()
    , model_(0)
    , skip_select_(false)
    , playable_index_()
{
    model_ = new Misc::StandardItemModel(this);
    model_->setColumnCount(2);
    model_->setHorizontalHeaderItem(0, new QStandardItem("Name"));
    model_->setHorizontalHeaderItem(1, new QStandardItem("Path"));

    setSoundFiles(sound_files);

    setModel(model_);
    setAcceptDrops(true);
    setEditable(false);
    setSelectionMode(QAbstractItemView::MultiSelection);
}

ListView::ListView(QWidget *parent)
    : QListView(parent)
    , start_pos_()
    , model_(0)
    , skip_select_(false)
    , playable_index_()
{
    model_ = new Misc::StandardItemModel(this);
    model_->setColumnCount(2);
    model_->setHorizontalHeaderItem(0, new QStandardItem("Name"));
    model_->setHorizontalHeaderItem(1, new QStandardItem("Path"));

    setModel(model_);
    setAcceptDrops(true);
    setEditable(false);
    setSelectionMode(QAbstractItemView::MultiSelection);
}

ListView::~ListView()
{}

void ListView::setSoundFiles(const QList<DB::SoundFileRecord *>& sound_files)
{
    model_->clear();
    foreach(DB::SoundFileRecord* rec, sound_files)
        addSoundFile(rec);
}

void ListView::setEditable(bool is_editable)
{
    for(int i = 0; i < model_->columnCount(); ++i)
        model_->setColumnEditable(i, is_editable);
}

bool ListView::getEditable()
{
    return model_->getColumnEditable(0);
}


QItemSelectionModel::SelectionFlags ListView::selectionCommand(const QModelIndex &index, const QEvent *event) const
{
    if (event != 0 && event->type() == QEvent::MouseMove)
        return QItemSelectionModel::Select;
    else
        return QAbstractItemView::selectionCommand(index, event);
}

void ListView::mousePressEvent(QMouseEvent *event)
{
    QModelIndex idx = indexAt(event->pos());
    if(idx.row() == -1 && idx.column() == -1)
        selectionModel()->clear();
    if (event->button() == Qt::LeftButton)
        start_pos_ = event->pos();
    QListView::mousePressEvent(event);
}

void ListView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        int distance = (event->pos() - start_pos_).manhattanLength();
        if (distance >= QApplication::startDragDistance())
            performDrag();
    }
    QListView::mouseMoveEvent(event);
}

void ListView::dragEnterEvent(QDragEnterEvent *event)
{
    ListView *source = qobject_cast<ListView*>(event->source());
    if (source && source != this) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void ListView::dragMoveEvent(QDragMoveEvent *event)
{
    ListView *source = qobject_cast<ListView *>(event->source());
    if (source && source != this) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void ListView::dropEvent(QDropEvent *event)
{
    ListView *source = qobject_cast<ListView*>(event->source());
    if (source && source != this) {
        // extract DB::TableRecord from mime data
        QList<DB::TableRecord*> records = Misc::JsonMimeDataParser::toTableRecordList(event->mimeData());

        // validate parsing
        if(records.size() == 0) {
            event->ignore();
            return;
        }

        // handle extracted data
        foreach(DB::TableRecord* rec, records) {
            if(rec->index == DB::SOUND_FILE) {
                DB::SoundFileRecord* sound_rec = (DB::SoundFileRecord*) rec;
                addSoundFile(sound_rec->id, sound_rec->name, sound_rec->path);
            }
        }

        // delete temp records
        while(records.size() > 0) {
            delete records[0];
            records.pop_front();
        }

        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void ListView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    if(skip_select_) {
        skip_select_ = false;
        selectionModel()->clearSelection();
        setSelection(rect, QItemSelectionModel::Deselect);
        return;
    }
    QListView::setSelection(rect, command);
}

void ListView::addSoundFile(DB::SoundFileRecord *rec)
{
    addSoundFile(rec->id, rec->name, rec->path);
}

void ListView::onSoundFileAboutToBeDeleted(DB::SoundFileRecord *)
{
    qDebug() << "TODO implement sound file about to be deleted.";
}

void ListView::onDropSuccessful()
{
    QCoreApplication::processEvents();
    skip_select_ = true;
}

void ListView::addSoundFile(int id, const QString &name, const QString &path)
{
    QList<QStandardItem*> items;
    items.push_back(new QStandardItem(name));
    items.push_back(new QStandardItem(path));
    model_->appendRow(items);
    QModelIndex idx = model_->index(model_->rowCount()-1, 0);
    model_->setData(idx, QVariant(id), Qt::UserRole);
}

void ListView::performDrag()
{
    QList<DB::TableRecord*> records;
    foreach(QModelIndex idx, selectionModel()->selectedIndexes()) {
        DB::SoundFileRecord* temp_rec = new DB::SoundFileRecord;
        temp_rec->id = model_->data(model_->index(idx.row(), 0), Qt::UserRole).toInt();
        temp_rec->name = model_->data(model_->index(idx.row(), 0)).toString();
        temp_rec->path = model_->data(model_->index(idx.row(), 1)).toString();
        records.append(temp_rec);
    }

    if(records.size() == 0)
        return;

    selectionModel()->clear();

    // create QMimeData
    QMimeData* mime_data = Misc::JsonMimeDataParser::toJsonMimeData(records);

    // delete temporary TableRecords
    while(records.size() > 0) {
        delete records[0];
        records.pop_front();
    }

    // create Drag
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mime_data);
    drag->setPixmap(*Resources::Lib::PX_SOUND_FILE_DRAG);

    // will block until drag done
    drag->exec(Qt::CopyAction);
}

} // namespace SoundFile
