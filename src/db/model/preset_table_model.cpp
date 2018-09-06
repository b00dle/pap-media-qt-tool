#include "preset_table_model.h"

#include <QCoreApplication>
#include <QDebug>

namespace DB {
namespace Model {

PresetTableModel::PresetTableModel(Core::Api* api, QObject* parent)
    : QAbstractTableModel(parent)
    , api_(api)
    , source_model_(0)
    , records_()
{}

PresetTableModel::~PresetTableModel()
{
    clear();
}

int PresetTableModel::columnCount(const QModelIndex&) const
{
    return 2; // (id/)name, json
}

int PresetTableModel::rowCount(const QModelIndex&) const
{
    return records_.size();
}

QVariant PresetTableModel::data(const QModelIndex &index, int role) const
{
    if(!indexIsValid(index))
        return QVariant();

    if(role == Qt::DisplayRole) {
        if(index.column() == 0)
            return QVariant(records_[index.row()]->name);
        else if(index.column() == 1)
            return QVariant(records_[index.row()]->json);
    }
    else if(role == Qt::EditRole) {
        if(index.column() == 0)
            return QVariant(records_[index.row()]->id);
        else if(index.column() == 1)
            return QVariant(records_[index.row()]->json);
    }

    return QVariant();
}

QVariant PresetTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // horizontal header (attribute names)
    if(orientation == Qt::Horizontal) {
        if(section < 0 || section >= columnCount())
            return QVariant();

        if(role == Qt::DisplayRole) {
            if(section == 0)
                return QVariant("Name");
            else if(section == 1)
                return QVariant("Json");
        }

        else if(role == Qt::EditRole) {
            if(section == 0)
                return QVariant("ID");
            else if(section == 1)
                return QVariant("Json");
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

Qt::ItemFlags PresetTableModel::flags(const QModelIndex &index) const
{
    if(!indexIsValid(index))
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index);
}

bool PresetTableModel::setData(const QModelIndex &, const QVariant &, int)
{
    // TODO support edit
    return false;
}

bool PresetTableModel::setHeaderData(int, Qt::Orientation, const QVariant &, int)
{
    // TODO support edit
    return false;
}

bool PresetTableModel::removeColumn(int, const QModelIndex&)
{
    return false;
}

bool PresetTableModel::removeColumns(int, int, const QModelIndex&)
{
    return false;
}

bool PresetTableModel::removeRow(int row, const QModelIndex&)
{
    if(row < 0)
        return false;

    // collect PresetRecord being deleted
    DB::PresetRecord* rec = getPresetByRow(row);
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

bool PresetTableModel::removeRows(int row, int count, const QModelIndex&)
{
    if(count < 0 || row < 0)
        return false;

    if(row < records_.size() && row+count < records_.size()) {

        // collect all PresetRecords being deleted
        QList<DB::PresetRecord*> recs;
        for(int i = row; i <= row+count; ++i) {
            DB::PresetRecord* rec = getPresetByRow(i);
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
            DB::PresetRecord* rec = recs.first();
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

void PresetTableModel::update()
{
    select();
}

void PresetTableModel::select()
{
    if(api_ == 0) {
        qDebug() << "FAILURE: cannot select PresetTableModel";
        qDebug() << " > (DB::Core::Api*) api is null";
        return;
    }

    if(records_.size() > 0)
        clear();

    source_model_ = api_->getPresetTable();

    int c_id = source_model_->fieldIndex("id");
    int c_name = source_model_->fieldIndex("name");
    int c_json = source_model_->fieldIndex("json");
    for(int row = 0; row < source_model_->rowCount(); ++row) {
        PresetRecord* rec = new PresetRecord;

        // set id
        QModelIndex index = source_model_->index(row, c_id);
        rec->id = source_model_->data(index).toInt();

        // set name
        index = source_model_->index(row, c_name);
        rec->name = source_model_->data(index).toString();

        // set json
        index = source_model_->index(row, c_json);
        rec->json = source_model_->data(index).toString();

        // add to list of records
        records_.append(rec);
    }

    emit layoutAboutToBeChanged();
    emit layoutChanged();
    emit dataChanged(index(0,0), index(rowCount(), columnCount()));
}

int PresetTableModel::getRowByPreset(PresetRecord *rec)
{
    for(int row = 0; row < records_.size(); ++row) {
        if(records_[row] == rec)
            return row;
    }
    return -1;
}

PresetRecord *PresetTableModel::getPresetById(int id)
{
    foreach(PresetRecord* rec, records_) {
        if(rec->id == id)
            return rec;
    }

    return 0;
}

PresetRecord *PresetTableModel::getPresetByName(const QString &name)
{
    foreach(PresetRecord* rec, records_) {
        if(rec->name.compare(name) == 0)
            return rec;
    }

    return 0;
}

PresetRecord *PresetTableModel::getPresetByRow(int row)
{
    if(row < rowCount())
        return records_[row];

    return 0;
}

PresetRecord *PresetTableModel::getLastPresetRecord()
{
    if(rowCount() > 0)
        return getPresetByRow(rowCount() - 1);

    return 0;
}

void PresetTableModel::addPresetRecord(const QString& name, const QString& json)
{
   api_->insertPreset(name, json);

    int id = api_->getPresetId(name);
    if(id == -1) {
        qDebug() << "FAILURE: Unknown error adding PresetRecord";
        qDebug() << " > json:" << json;
        return;
    }

    records_.append(new PresetRecord(id, name, json));

    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

const QList<PresetRecord *> &PresetTableModel::getPresets() const
{
    return records_;
}

void PresetTableModel::deletePreset(int id)
{
    PresetRecord* rec = getPresetById(id);
    if(rec == 0)
        return;

    int row = getRowByPreset(rec);

    if(row != -1)
        removeRow(row);
}

bool PresetTableModel::indexIsValid(const QModelIndex & index) const
{
    return index.isValid() && index.row() < rowCount() && index.column() < columnCount();
}

void PresetTableModel::clear()
{
    while(records_.size() > 0) {
        PresetRecord* rec = records_.front();
        records_.pop_front();
        delete rec;
    }
}

} // namespace Model
} // namespace DB

