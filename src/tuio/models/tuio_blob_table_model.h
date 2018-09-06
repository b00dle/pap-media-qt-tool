#ifndef TUIO_BLOB_TABLE_MODEL_H
#define TUIO_BLOB_TABLE_MODEL_H

#include <QAbstractTableModel>

#include "qtuioblob_p.h"

/**
 * Table model to maintain QTuioBlobs.
 * Editting through regular table views is disabled.
 * Functionality to add/update/remove blobs is provided.
 * Can be connected to QTuioHandler::blobEvent(...)
 * (see onBlobEvent(...) slot) for bulk update and easy maintanance.
 * Use QPersistentModelIndex to keep track of QTuioBlobs managed here,
 * or track desired id and lookup row using utility functions provided.
*/
class TuioBlobTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum BlobChange {
        BLOB_ADDED,
        BLOB_UPDATED,
        BLOB_REMOVED
    };

public:
    explicit TuioBlobTableModel(QObject *parent = nullptr);

    /**
     * Returns the number of rows in this table model.
     * Parent has to be invalid model index (QModelIndex()),
     * since there is no hierarchy in this model.
    */
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * Returns the number of columns in this table model.
     * Parent has to be invalid model index (QModelIndex()),
     * since there is no hierarchy in this model.
    */
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * Returns the model data associated with given model index.
    */
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    /**
     * Returns the header data associated with given column and orientation.
    */
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

    /**
     * Returns the item flags for given model index.
    */
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    /**
     * Returns the drag actions flags for this model.
    */
    virtual Qt::DropActions supportedDragActions() const;

    /**
     * Returns the drop actions flags for this model.
    */
    virtual Qt::DropActions supportedDropActions() const;

    /**
     * Returns the row for blob referenced by id,
     * or -1 if no blob with given id is managed by this model.
    */
    int getRow(int id) const;

    /**
     * Returns the row for given blob,
     * or -1 if no blob with given id is managed by this model.
    */
    int getRow(const QTuioBlob& c) const;

    /**
     * Returns a copy to the blob referenced by given id.
     * Id of returned blob will be -1 if given id
     * does not reference a blob managed by this model.
    */
    const QTuioBlob getBlob(int id) const;

    /**
     * Returns true if a QTuioBlob with given id is maintained by this model.
     * False otherwise.
    */
    bool hasBlob(int id) const;

    /**
     * Adds given blob to the list of blobs maintained by the model.
     * Returns true if blob added.
     * If a blob with id of given blob already exists, no new entry
     * will be created and false is returned.
    */
    bool addBlob(const QTuioBlob&);

    /**
     * Adds a placeholder QTuioBlob with given id and
     * default property values into the model.
     * Returns true if blob created, or false
     * if blob already exists in model.
    */
    bool addBlob(int id);

    /**
     * Updates the values of blob in model identified by given blob.id
     * with all other values contained by given blob.
     * Returns true if blob updated, or false if blob
     * does not exist.
    */
    bool updateBlob(const QTuioBlob&);

    /**
     * Removes blob referenced by given id.
     * Returns true if removal successful, false otherwise.
    */
    bool removeBlob(int id);

signals:
    void blobChanged(int id, TuioBlobTableModel::BlobChange change);

public slots:
    /**
     * Handles bulk update of model.
     * Adds or updates all blobs in active_blobs and
     * deletes all blobs in dead_blobs if contained by model.
    */
    void onBlobEvent(const QMap<int, QTuioBlob>& active_blobs,
                       const QVector<QTuioBlob>& dead_blobs);

protected:
    /**
     * Helper function to access properties of QTuioBlob by field number.
    */
    QVariant data(const QTuioBlob& blob, int field) const;

    /**
     * Helper function to access names of QTuioBlob fields by field number.
    */
    QVariant horizontalHeaderData(int field) const;

    /**
     * Internal function to add blob to the model.
     * emits calls to update layout, etc.
     * Assumes prior validation of operation.
    */
    void internalAddBlob(const QTuioBlob& c);

    /**
     * Internal function to add blob to the model.
     * emits calls to update layout, etc.
     * Assumes prior validation of operation.
    */
    void internalUpdateBlob(const QTuioBlob& c);

    /**
     * Internal function to remove blob from the model.
     * emits calls to update layout, etc.
     * Assumes prior validation of operation.
    */
    void internalRemoveBlob(int id);

    /**
     * Returns true if given QModelIndex is valid within the model instance.
     * False otherwise.
    */
    bool isValidDataIndex(const QModelIndex& idx) const;

    /**
     * Returns true if given QModelIndex is valid within
     * the model instances horizontal header section.
     * False otherwise.
    */
    bool isValidHeaderIndex(const QModelIndex& idx) const;

    QMap<int, QTuioBlob> blobs_;

};

#endif // TUIO_BLOB_TABLE_MODEL_H
