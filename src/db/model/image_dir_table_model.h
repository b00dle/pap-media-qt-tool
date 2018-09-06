#ifndef DB_MODEL_IMAGE_DIR_TABLE_MODEL_H
#define DB_MODEL_IMAGE_DIR_TABLE_MODEL_H

#include <QAbstractTableModel>
#include "db/core/api.h"
#include "db/table_records.h"

namespace DB {
namespace Model {

class ImageDirTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ImageDirTableModel(Core::Api* api, QObject *parent = 0);
    ~ImageDirTableModel();

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
     * Gets the row of ImageDirRecord.
     * Returns -1 if none found
    */
    int getRowByImageDir(DB::ImageDirRecord* rec);

    /*
     * Gets ImageDirRecord based on path.
     * Returns 0 if none found.
    */
    ImageDirRecord* getImageDirByPath(QString const& path);

    /*
     * Gets ImageDirRecord based on ID.
     * Returns 0 if none found.
    */
    ImageDirRecord* getImageDirById(int id);

    /*
    * Gets ImageDirRecord based on row.
    * Returns 0 if none found.
    */
    ImageDirRecord* getImageDirByRow(int row);

    /*
     * Gets last ImageDirRecord in the model.
     * Returns 0 if none found.
    */
    ImageDirRecord* getLastImageDirRecord();

    /*
    * Adds a ImageDirRecord to this model
    */
    void addImageDirRecord(const QFileInfo& info);

    /*
    * Returns all ImageDirRecords held by this model
    */
    QList<DB::ImageDirRecord*> const& getImageDirs() const;


signals:   
   /* triggered and processed before ImageDirRecord gets deleted */
   void aboutToBeDeleted(DB::ImageDirRecord*);

   /* triggered and processed before ImageDirRecords get deleted */
   void aboutToBeDeleted(const QList<DB::ImageDirRecord*>&);

public slots:
    void deleteImageDir(int id);

private:
    /* validates existance of given QModelIndex for this model **/
    bool indexIsValid(const QModelIndex&) const;

    /* Clears all ImageDirRecords from records **/
    void clear();

    Core::Api* api_;
    QSqlRelationalTableModel* source_model_;
    QList<ImageDirRecord*> records_;
};

} // namepace Model
} // namespace DB

#endif // DB_MODEL_IMAGE_DIR_TABLE_MODEL_H
