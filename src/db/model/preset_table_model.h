#ifndef DB_MODEL_PRESET_TABLE_MODEL_H
#define DB_MODEL_PRESET_TABLE_MODEL_H

#include <QAbstractTableModel>
#include "db/core/api.h"
#include "db/table_records.h"

namespace DB {
namespace Model {

class PresetTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit PresetTableModel(Core::Api* api, QObject *parent = 0);
    ~PresetTableModel();

    //// inheritted functions (from pure virtual BC) - see docs for description

    // read
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    //write
    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant &value, int role = Qt::DisplayRole);

    /* does nothing, returns false */
    bool removeColumn(int column, const QModelIndex &parent = QModelIndex());

    /* does nothing, returns false */
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex());

    bool removeRow(int row, const QModelIndex &parent = QModelIndex());

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    //// end inheritted functions

    /* Update model with data from db */
    void update();

    /* Fills model with data from SoundFile database table **/
    void select();

    /*
     * Gets the row of PresetRecord.
     * Returns -1 if none found
    */
    int getRowByPreset(DB::PresetRecord* rec);

    /*
     * Gets PresetRecord based on ID.
     * Returns 0 if none found.
    */
    PresetRecord* getPresetById(int id);

    /*
     * Gets PresetRecord based on name.
     * Returns 0 if none found.
    */
    PresetRecord* getPresetByName(QString const& name);

    /*
    * Gets PresetRecord based on row.
    * Returns 0 if none found.
    */
    PresetRecord* getPresetByRow(int row);

    /*
     * Gets last PresetRecord in the model.
     * Returns 0 if none found.
    */
    PresetRecord* getLastPresetRecord();

    /*
    * Adds a PresetRecord to this model
    */
    void addPresetRecord(const QString& name, const QString& json);

    /*
    * Returns all PresetRecords held by this model
    */
    QList<DB::PresetRecord*> const& getPresets() const;


signals:   
   /* triggered and processed before PresetRecord gets deleted */
   void aboutToBeDeleted(DB::PresetRecord*);

   /* triggered and processed before PresetRecords get deleted */
   void aboutToBeDeleted(const QList<DB::PresetRecord*>&);

public slots:
    void deletePreset(int id);

private:
    /* validates existance of given QModelIndex for this model **/
    bool indexIsValid(const QModelIndex&) const;

    /* Clears all PresetRecords from records **/
    void clear();

    Core::Api* api_;
    QSqlRelationalTableModel* source_model_;
    QList<PresetRecord*> records_;
};

} // namepace Model
} // namespace DB

#endif // DB_MODEL_PRESET_TABLE_MODEL_H
