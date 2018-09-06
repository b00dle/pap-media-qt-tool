#include "tuio_cursor_table_model.h"

#include <QCoreApplication>
#include <QSet>

TuioCursorTableModel::TuioCursorTableModel(QObject *parent)
    : QAbstractTableModel(parent)
    , cursors_()
{
}

int TuioCursorTableModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    return cursors_.size();
}

int TuioCursorTableModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    return 7;
}

QVariant TuioCursorTableModel::data(const QModelIndex &index, int role) const
{
    if(role != Qt::DisplayRole || !isValidDataIndex(index))
        return QVariant();
    return data(cursors_[cursors_.keys()[index.row()]], index.column());
}

QVariant TuioCursorTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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

Qt::ItemFlags TuioCursorTableModel::flags(const QModelIndex &index) const
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

Qt::DropActions TuioCursorTableModel::supportedDragActions() const
{
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction | Qt::IgnoreAction | Qt::TargetMoveAction;
}

Qt::DropActions TuioCursorTableModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction | Qt::IgnoreAction | Qt::TargetMoveAction;
}

int TuioCursorTableModel::getRow(int id) const
{
    if(!hasCursor(id))
        return -1;
    return cursors_.keys().indexOf(id);
}

int TuioCursorTableModel::getRow(const QTuioCursor &c) const
{
    return getRow(c.id());
}

const QTuioCursor TuioCursorTableModel::getCursor(int id) const
{
    if(!hasCursor(id))
        return QTuioCursor();
    return cursors_[id];
}

bool TuioCursorTableModel::hasCursor(int id) const
{
    return cursors_.contains(id);
}

bool TuioCursorTableModel::addCursor(const QTuioCursor& c)
{
    if(hasCursor(c.id()))
        return false;
    internalAddCursor(c);
    return true;
}

bool TuioCursorTableModel::addCursor(int id)
{
    if(hasCursor(id))
        return false;
    QTuioCursor c(id);
    internalAddCursor(c);
    return true;
}

bool TuioCursorTableModel::updateCursor(const QTuioCursor &c)
{
    if(!hasCursor(c.id()))
        return false;
    internalUpdateCursor(c);
    return true;
}

bool TuioCursorTableModel::removeCursor(int id)
{
    if(!hasCursor(id))
        return false;
    internalRemoveCursor(id);
    return true;
}

void TuioCursorTableModel::onCursorEvent(const QMap<int, QTuioCursor> &active_cursors, const QVector<QTuioCursor>& dead_cursors)
{
    foreach(auto c, active_cursors.values()) {
        if(hasCursor(c.id()))
            updateCursor(c);
        else
            addCursor(c);
    }
    foreach(auto c, dead_cursors) {
        if(hasCursor(c.id()))
            removeCursor(c.id());
    }
}

QVariant TuioCursorTableModel::data(const QTuioCursor &cursor, int field) const
{
    switch(field) {
        case 0: return QVariant(cursor.id());
        case 1: return QVariant(cursor.x());
        case 2: return QVariant(cursor.y());
        case 3: return QVariant(cursor.vx());
        case 4: return QVariant(cursor.vy());
        case 5: return QVariant(cursor.acceleration());
        case 6: return QVariant(cursor.state());
        default: return QVariant();
    }
}

QVariant TuioCursorTableModel::horizontalHeaderData(int field) const
{
    switch(field) {
        case 0: return QVariant("ID");
        case 1: return QVariant("X");
        case 2: return QVariant("Y");
        case 3: return QVariant("VX");
        case 4: return QVariant("VY");
        case 5: return QVariant("Acceleration");
        case 6: return QVariant("State");
        default: return QVariant();
    }
}

void TuioCursorTableModel::internalAddCursor(const QTuioCursor &c)
{
    QSet<int> keys = cursors_.keys().toSet();
    keys.insert(c.id());
    int row = keys.toList().indexOf(c.id());
    beginInsertRows(QModelIndex(), row, row);
    cursors_[c.id()] = c;
    endInsertRows();
    emit cursorChanged(c.id(), CURSOR_ADDED);
}

void TuioCursorTableModel::internalUpdateCursor(const QTuioCursor &c)
{
    cursors_[c.id()].setX(c.x());
    cursors_[c.id()].setY(c.y());
    cursors_[c.id()].setVX(c.vx());
    cursors_[c.id()].setVY(c.vy());
    cursors_[c.id()].setAcceleration(c.acceleration());
    cursors_[c.id()].setState(c.state());
    int row = getRow(c.id());
    emit dataChanged(index(row, 0), index(row, columnCount()-1));
    emit cursorChanged(c.id(), CURSOR_UPDATED);
}

void TuioCursorTableModel::internalRemoveCursor(int id)
{
    int row = getRow(id);
    beginRemoveRows(QModelIndex(), row, row);
    cursors_.remove(id);
    endRemoveRows();
    emit cursorChanged(id, CURSOR_REMOVED);
}

bool TuioCursorTableModel::isValidDataIndex(const QModelIndex &idx) const
{
    if(!idx.isValid())
        return false;
    if(idx.row() >= rowCount() || idx.row() < 0)
        return false;
    if(idx.column() >= columnCount() || idx.row() < 0)
        return false;
    return true;
}

bool TuioCursorTableModel::isValidHeaderIndex(const QModelIndex &idx) const
{
    if(!idx.isValid())
        return false;
    return (idx.row() >= 0 && idx.row() < rowCount() && idx.column() == -1) ||
           (idx.column() >= 0 && idx.column() < columnCount() && idx.row() == -1);
}
