#include "tracker_table_model.h"

#include <QDebug>

TrackerTableModel::TrackerTableModel(QObject *parent)
    : QAbstractTableModel(parent)
    , trackers_()
{}

TrackerTableModel::~TrackerTableModel()
{
    foreach(auto tracker, trackers_.values())
        delete tracker;
}

int TrackerTableModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    return trackers_.size();
}

int TrackerTableModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    return 5;
}

QVariant TrackerTableModel::data(const QModelIndex &index, int role) const
{
    if(role != Qt::DisplayRole || !isValidDataIndex(index))
        return QVariant();
    return data(trackers_[trackers_.keys()[index.row()]], index.column());
}

QVariant TrackerTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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

Qt::ItemFlags TrackerTableModel::flags(const QModelIndex &index) const
{
    bool is_data_idx = isValidDataIndex(index);
    bool is_header_idx = isValidHeaderIndex(index);
    if(!is_data_idx && !is_header_idx)
        return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

Qt::DropActions TrackerTableModel::supportedDragActions() const
{
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction | Qt::IgnoreAction | Qt::TargetMoveAction;
}

Qt::DropActions TrackerTableModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction | Qt::IgnoreAction | Qt::TargetMoveAction;
}

int TrackerTableModel::getRow(const QString &name) const
{
    if(!hasTracker(name))
        return -1;
    return trackers_.keys().indexOf(name);
}

int TrackerTableModel::getRow(const Tracker &t) const
{
    return getRow(t.getName());
}

Tracker *TrackerTableModel::getTracker(const QString &name) const
{
    if(!hasTracker(name))
        return nullptr;
    return trackers_[name];
}

const QList<Tracker *> TrackerTableModel::getTrackers() const
{
    return trackers_.values();
}

bool TrackerTableModel::hasTracker(const QString &name) const
{
    return trackers_.contains(name);
}

bool TrackerTableModel::hasTracker(Tracker *tracker) const
{
    foreach(auto t, trackers_.values()) {
        if(t == tracker)
            return true;
    }
    return false;
}

bool TrackerTableModel::addTracker(Tracker* t)
{
    if(hasTracker(t->getName()))
        return false;
    internalAddTracker(t);
    return true;
}

bool TrackerTableModel::updateTracker(Tracker* t)
{
    if(!hasTracker(t->getName()))
        return false;
    internalUpdateTracker(t);
    return true;
}

bool TrackerTableModel::removeTracker(const QString &name)
{
    if(!hasTracker(name))
        return false;
    internalRemoveTracker(name);
    return true;
}

bool TrackerTableModel::removeTracker(Tracker *t)
{
    if(!hasTracker(t))
        return false;
    foreach(auto name, trackers_.keys()) {
        if(trackers_[name] == t)
            return removeTracker(name);
    }
    return false;
}

QVariant TrackerTableModel::data(Tracker *tracker, int field) const
{
    switch(field) {
        case 0: return QVariant(tracker->getName());
        case 1: return QVariant(toString(tracker->getRelativePosition()));
        case 2: return QVariant(toString(tracker->getPosition()));
        case 3: return QVariant(QString::number(tracker->getRotation()));
        case 4: return QVariant(QString::number(tracker->trackerType()));
        default: return QVariant();
    }
}

QVariant TrackerTableModel::horizontalHeaderData(int field) const
{
    switch(field) {
        case 0: return QVariant("Tracker Name");
        case 1: return QVariant("Relative Position");
        case 2: return QVariant("Position");
        case 3: return QVariant(QString(QChar(0xb1, 0x03)));
        case 4: return QVariant("Tracker Type");
        default: return QVariant();
    }
}

void TrackerTableModel::internalAddTracker(Tracker* t)
{
    QSet<QString> keys = trackers_.keys().toSet();
    keys.insert(t->getName());
    int row = keys.toList().indexOf(t->getName());
    beginInsertRows(QModelIndex(), row, row);
    trackers_[t->getName()] = t;
    endInsertRows();
    emit trackerAdded(t->getName());
}

void TrackerTableModel::internalUpdateTracker(Tracker* t)
{
    trackers_[t->getName()]->set(t);
    int row = getRow(t->getName());
    emit dataChanged(index(row, 0), index(row, columnCount()-1));
}

void TrackerTableModel::internalRemoveTracker(const QString &name)
{
    int row = getRow(name);
    beginRemoveRows(QModelIndex(), row, row);
    if(trackers_[name]->getCleanByModelEnabled())
        delete trackers_[name];
    else
        trackers_[name]->removeAllTracking();
    trackers_.remove(name);
    endRemoveRows();
    emit trackerRemoved(name);
}

bool TrackerTableModel::isValidDataIndex(const QModelIndex &idx) const
{
    if(!idx.isValid())
        return false;
    if(idx.row() >= rowCount() || idx.row() < 0)
        return false;
    if(idx.column() >= columnCount() || idx.row() < 0)
        return false;
    return true;
}

bool TrackerTableModel::isValidHeaderIndex(const QModelIndex &idx) const
{
    if(!idx.isValid())
        return false;
    return (idx.row() >= 0 && idx.row() < rowCount() && idx.column() == -1) ||
           (idx.column() >= 0 && idx.column() < columnCount() && idx.row() == -1);
}

const QString TrackerTableModel::toString(const QPointF &p)
{
    return "[" + QString::number(p.x()) + "," + QString::number(p.y()) + "]";
}
