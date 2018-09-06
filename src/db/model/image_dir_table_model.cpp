#include "image_dir_table_model.h"

#include <QCoreApplication>
#include <QDebug>

namespace DB {
namespace Model {

ImageDirTableModel::ImageDirTableModel(Core::Api* api, QObject* parent)
    : QAbstractTableModel(parent)
    , api_(api)
    , source_model_(0)
    , records_()
{}

ImageDirTableModel::~ImageDirTableModel()
{
    clear();
}

int ImageDirTableModel::columnCount(const QModelIndex&) const
{
    return 1; // (id/)name, path
}

int ImageDirTableModel::rowCount(const QModelIndex&) const
{
    return records_.size();
}

QVariant ImageDirTableModel::data(const QModelIndex &index, int role) const
{
    if(!indexIsValid(index))
        return QVariant();

    if(role == Qt::DisplayRole) {
        if(index.column() == 0)
            return QVariant(records_[index.row()]->path);
    }
    else if(role == Qt::EditRole) {
        if(index.column() == 0)
            return QVariant(records_[index.row()]->id);
    }

    return QVariant();
}

QVariant ImageDirTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // horizontal header (attribute names)
    if(orientation == Qt::Horizontal) {
        if(section < 0 || section >= columnCount())
            return QVariant();

        if(role == Qt::DisplayRole) {
            if(section == 0)
                return QVariant("Recent Directories");
        }

        else if(role == Qt::EditRole) {
            if(section == 0)
                return QVariant("ID");
        }
    }

    // vertical header (number of record)
    else {
        if(section < 0 || section >= rowCount())
            return QVariant();
        if(role == Qt::DisplayRole || role == Qt::EditRole)
            return QVariant(section);
    }

    return QVariant();
}

Qt::ItemFlags ImageDirTableModel::flags(const QModelIndex &index) const
{
    if(!indexIsValid(index))
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index);
}

bool ImageDirTableModel::setData(const QModelIndex &, const QVariant &, int)
{
    // TODO support edit
    return false;
}

bool ImageDirTableModel::setHeaderData(int, Qt::Orientation, const QVariant &, int)
{
    // TODO support edit
    return false;
}

bool ImageDirTableModel::removeColumn(int, const QModelIndex&)
{
    return false;
}

bool ImageDirTableModel::removeColumns(int, int, const QModelIndex&)
{
    return false;
}

bool ImageDirTableModel::removeRow(int row, const QModelIndex&)
{
    if(row < 0)
        return false;

    // collect ImageDirRecord being deleted
    DB::ImageDirRecord* rec = getImageDirByRow(row);
    if(rec != 0) {

        // signal deletion
        emit aboutToBeDeleted(rec);
        QCoreApplication::processEvents();

        // remove from source
        if(!source_model_->removeRow(row, QModelIndex()))
            return false;

        // remove from storage & delete pointer
        records_.removeAt(row);
        delete rec;
        rec = 0;

        // call super class function
        QAbstractTableModel::removeRow(row, QModelIndex());

        return true;
    }

    return false;
}

bool ImageDirTableModel::removeRows(int row, int count, const QModelIndex&)
{
    if(count < 0 || row < 0)
        return false;

    if(row < records_.size() && row+count < records_.size()) {

        // collect all ImageDirRecords being deleted
        QList<DB::ImageDirRecord*> recs;
        for(int i = row; i <= row+count; ++i) {
            DB::ImageDirRecord* rec = getImageDirByRow(i);
            if(rec == 0)
                return false;
            recs.append(rec);
        }

        // signal deletion
        emit aboutToBeDeleted(recs);
        QCoreApplication::processEvents();

        // remove from source model
        if(!source_model_->removeRows(row, count, QModelIndex()))
            return false;

        // remove from storage
        for(int i = row+count; i >= row; --i)
            records_.removeAt(i);

        // delete pointers
        while(recs.size() > 0) {
            DB::ImageDirRecord* rec = recs.first();
            delete rec;
            rec = 0;
            recs.pop_front();
        }

        // call super class function
        QAbstractTableModel::removeRows(row, count, QModelIndex());

        return true;
    }

    return false;
}

void ImageDirTableModel::update()
{
    select();
}

void ImageDirTableModel::select()
{
    if(api_ == 0) {
        qDebug() << "FAILURE: cannot select ImageDirTableModel";
        qDebug() << " > (DB::Core::Api*) api is null";
        return;
    }

    if(records_.size() > 0)
        clear();

    source_model_ = api_->getImageDirTable();

    int c_id = source_model_->fieldIndex("id");
    int c_path = source_model_->fieldIndex("path");
    for(int row = 0; row < source_model_->rowCount(); ++row) {
        ImageDirRecord* rec = new ImageDirRecord;

        // set id
        QModelIndex index = source_model_->index(row, c_id);
        rec->id = source_model_->data(index).toInt();

        // set path
        index = source_model_->index(row, c_path);
        rec->path = source_model_->data(index).toString();

        // add to list of records
        records_.append(rec);
    }

    emit layoutAboutToBeChanged();
    emit layoutChanged();
    emit dataChanged(index(0,0), index(rowCount(), columnCount()));
}

int ImageDirTableModel::getRowByImageDir(ImageDirRecord *rec)
{
    for(int row = 0; row < records_.size(); ++row) {
        if(records_[row] == rec)
            return row;
    }
    return -1;
}

ImageDirRecord *ImageDirTableModel::getImageDirByPath(const QString &path)
{
    foreach(ImageDirRecord* rec, records_) {
        if(rec->path.compare(path) == 0)
            return rec;
    }

    return 0;
}

ImageDirRecord *ImageDirTableModel::getImageDirById(int id)
{
    foreach(ImageDirRecord* rec, records_) {
        if(rec->id == id)
            return rec;
    }

    return 0;
}

ImageDirRecord *ImageDirTableModel::getImageDirByRow(int row)
{
    if(row < rowCount())
        return records_[row];

    return 0;
}

ImageDirRecord *ImageDirTableModel::getLastImageDirRecord()
{
    if(rowCount() > 0)
        return getImageDirByRow(rowCount() - 1);

    return 0;
}

void ImageDirTableModel::addImageDirRecord(const QFileInfo &info)
{
    if(!info.isDir()) {
        qDebug() << "FAILURE: cannot add ImageDirRecord.";
        qDebug() << " > file info specified (" << info.filePath() << ") is not a directory.";
        return;
    }

    if(getImageDirByPath(info.filePath()) != 0) {
        qDebug() << "FAILURE: cannot add ImageDirRecord.";
        qDebug() << " > ImageDir with path" << info.filePath() << "already exists.";
        return;
    }

    api_->insertImageDir(info);

    int id = api_->getImageDirId(info.filePath());
    if(id == -1) {
        qDebug() << "FAILURE: Unknown error adding ImageDirRecord";
        qDebug() << " > path:" << info.filePath();
        return;
    }

    records_.append(new ImageDirRecord(id, info.fileName(), info.filePath()));

    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

const QList<ImageDirRecord *> &ImageDirTableModel::getImageDirs() const
{
    return records_;
}

void ImageDirTableModel::deleteImageDir(int id)
{
    ImageDirRecord* rec = getImageDirById(id);
    if(rec == 0)
        return;

    int row = getRowByImageDir(rec);

    if(row != -1)
        removeRow(row);
}

bool ImageDirTableModel::indexIsValid(const QModelIndex & index) const
{
    return index.isValid() && index.row() < rowCount() && index.column() < columnCount();
}

void ImageDirTableModel::clear()
{
    while(records_.size() > 0) {
        ImageDirRecord* rec = records_.front();
        records_.pop_front();
        delete rec;
    }
}

} // namespace Model
} // namespace DB

