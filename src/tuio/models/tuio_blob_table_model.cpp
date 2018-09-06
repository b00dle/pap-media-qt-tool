#include "tuio_blob_table_model.h"

#include <QCoreApplication>
#include <QSet>

TuioBlobTableModel::TuioBlobTableModel(QObject *parent)
    : QAbstractTableModel(parent)
    , blobs_()
{
}

int TuioBlobTableModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    return blobs_.size();
}

int TuioBlobTableModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    return 11;
}

QVariant TuioBlobTableModel::data(const QModelIndex &index, int role) const
{
    if(role != Qt::DisplayRole || !isValidDataIndex(index))
        return QVariant();
    return data(blobs_[blobs_.keys()[index.row()]], index.column());
}

QVariant TuioBlobTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Vertical) {
        if(section >= rowCount())
            return QVariant();
        return QVariant(section);
    }
    else {
        if(section >= columnCount())
            return QVariant();
        return horizontalHeaderData(section);
    }
}

Qt::ItemFlags TuioBlobTableModel::flags(const QModelIndex &index) const
{
    bool is_data_idx = isValidDataIndex(index);
    bool is_header_idx = isValidHeaderIndex(index);
    if(!is_data_idx && !is_header_idx)
        return Qt::NoItemFlags;
    else if(is_data_idx) {
        if(index.column() == 0)
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    }
    else if(is_header_idx) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    }
    return Qt::ItemIsEnabled;
}

Qt::DropActions TuioBlobTableModel::supportedDragActions() const
{
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction | Qt::IgnoreAction | Qt::TargetMoveAction;
}

Qt::DropActions TuioBlobTableModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction | Qt::IgnoreAction | Qt::TargetMoveAction;
}

int TuioBlobTableModel::getRow(int id) const
{
    if(!hasBlob(id))
        return -1;
    return blobs_.keys().indexOf(id);
}

int TuioBlobTableModel::getRow(const QTuioBlob &c) const
{
    return getRow(c.id());
}

const QTuioBlob TuioBlobTableModel::getBlob(int id) const
{
    if(!hasBlob(id))
        return QTuioBlob();
    return blobs_[id];
}

bool TuioBlobTableModel::hasBlob(int id) const
{
    return blobs_.contains(id);
}

bool TuioBlobTableModel::addBlob(const QTuioBlob& c)
{
    if(hasBlob(c.id()))
        return false;
    internalAddBlob(c);
    return true;
}

bool TuioBlobTableModel::addBlob(int id)
{
    if(hasBlob(id))
        return false;
    QTuioBlob c(id);
    internalAddBlob(c);
    return true;
}

bool TuioBlobTableModel::updateBlob(const QTuioBlob &c)
{
    if(!hasBlob(c.id()))
        return false;
    internalUpdateBlob(c);
    return true;
}

bool TuioBlobTableModel::removeBlob(int id)
{
    if(!hasBlob(id))
        return false;
    internalRemoveBlob(id);
    return true;
}

void TuioBlobTableModel::onBlobEvent(const QMap<int, QTuioBlob> &active_blobs, const QVector<QTuioBlob>& dead_blobs)
{
    foreach(auto c, active_blobs.values()) {
        if(hasBlob(c.id()))
            updateBlob(c);
        else
            addBlob(c);
    }
    foreach(auto c, dead_blobs) {
        if(hasBlob(c.id()))
            removeBlob(c.id());
    }
}

QVariant TuioBlobTableModel::data(const QTuioBlob &blob, int field) const
{
    switch(field) {
        case 0  : return QVariant(blob.id());
        case 1  : return QVariant(blob.x());
        case 2  : return QVariant(blob.y());
        case 3  : return QVariant(blob.angle());
        case 4  : return QVariant(blob.width());
        case 5  : return QVariant(blob.height());
        case 6  : return QVariant(blob.area());
        case 7  : return QVariant(blob.vx());
        case 8  : return QVariant(blob.vy());
        case 9  : return QVariant(blob.vr());
        case 10 : return QVariant(blob.acceleration());
        case 11 : return QVariant(blob.rotationAcceleration());
        case 12 : return QVariant(blob.state());
        default : return QVariant();
    }
}

QVariant TuioBlobTableModel::horizontalHeaderData(int field) const
{
    switch(field) {
        case 0  : return QVariant("ID");
        case 1  : return QVariant("X");
        case 2  : return QVariant("Y");
        case 3  : return QVariant("Angle");
        case 4  : return QVariant("Width");
        case 5  : return QVariant("Height");
        case 6  : return QVariant("Area");
        case 7  : return QVariant("VX");
        case 8  : return QVariant("VY");
        case 9  : return QVariant("VR");
        case 10 : return QVariant("Acceleration");
        case 11 : return QVariant("Rotation Acceleration");
        case 12 : return QVariant("State");
        default : return QVariant();
    }
}

void TuioBlobTableModel::internalAddBlob(const QTuioBlob &c)
{
    QSet<int> keys = blobs_.keys().toSet();
    keys.insert(c.id());
    int row = keys.toList().indexOf(c.id());
    beginInsertRows(QModelIndex(), row, row);
    blobs_[c.id()] = c;
    endInsertRows();
    emit blobChanged(c.id(), BLOB_ADDED);
}

void TuioBlobTableModel::internalUpdateBlob(const QTuioBlob &c)
{
    blobs_[c.id()].setX(c.x());
    blobs_[c.id()].setY(c.y());
    blobs_[c.id()].setVX(c.vx());
    blobs_[c.id()].setVY(c.vy());
    blobs_[c.id()].setAcceleration(c.acceleration());
    blobs_[c.id()].setState(c.state());
    int row = getRow(c.id());
    emit dataChanged(index(row, 0), index(row, columnCount()-1));
    emit blobChanged(c.id(), BLOB_UPDATED);
}

void TuioBlobTableModel::internalRemoveBlob(int id)
{
    int row = getRow(id);
    beginRemoveRows(QModelIndex(), row, row);
    blobs_.remove(id);
    endRemoveRows();
    emit blobChanged(id, BLOB_REMOVED);
}

bool TuioBlobTableModel::isValidDataIndex(const QModelIndex &idx) const
{
    if(!idx.isValid())
        return false;
    if(idx.row() >= rowCount() || idx.row() < 0)
        return false;
    if(idx.column() >= columnCount() || idx.row() < 0)
        return false;
    return true;
}

bool TuioBlobTableModel::isValidHeaderIndex(const QModelIndex &idx) const
{
    if(!idx.isValid())
        return false;
    return (idx.row() >= 0 && idx.row() < rowCount() && idx.column() == -1) ||
            (idx.column() >= 0 && idx.column() < columnCount() && idx.row() == -1);
}
