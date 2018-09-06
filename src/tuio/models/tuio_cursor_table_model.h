#ifndef TUIO_CURSOR_TABLE_MODEL_H
#define TUIO_CURSOR_TABLE_MODEL_H

#include <QAbstractTableModel>

#include "qtuiocursor_p.h"

/**
 * Table model to maintain QTuioCursors.
 * Editting through regular table views is disabled.
 * Functionality to add/update/remove cursors is provided.
 * Can be connected to QTuioHandler::cursorEvent(...)
 * (see onCursorEvent(...) slot) for bulk update and easy maintanance.
 * Use QPersistentModelIndex to keep track of QTuioCursors managed here,
 * or track desired id and lookup row using utility functions provided.
*/
class TuioCursorTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum CursorChange {
        CURSOR_ADDED,
        CURSOR_UPDATED,
        CURSOR_REMOVED
    };

public:
    explicit TuioCursorTableModel(QObject *parent = nullptr);

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
     * Returns the row for cursor referenced by id,
     * or -1 if no cursor with given id is managed by this model.
    */
    int getRow(int id) const;

    /**
     * Returns the row for given cursor,
     * or -1 if no cursor with given id is managed by this model.
    */
    int getRow(const QTuioCursor& c) const;

    /**
     * Returns a copy to the cursor referenced by given id.
     * Id of returned cursor will be -1 if given id
     * does not reference a cursor managed by this model.
    */
    const QTuioCursor getCursor(int id) const;

    /**
     * Returns true if a QTuioCursor with given id is maintained by this model.
     * False otherwise.
    */
    bool hasCursor(int id) const;

    /**
     * Adds given cursor to the list of cursors maintained by the model.
     * Returns true if cursor added.
     * If a cursor with id of given cursor already exists, no new entry
     * will be created and false is returned.
    */
    bool addCursor(const QTuioCursor&);

    /**
     * Adds a placeholder QTuioCursor with given id and
     * default property values into the model.
     * Returns true if cursor created, or false
     * if cursor already exists in model.
    */
    bool addCursor(int id);

    /**
     * Updates the values of cursor in model identified by given cursor.id
     * with all other values contained by given cursor.
     * Returns true if cursor updated, or false if cursor
     * does not exist.
    */
    bool updateCursor(const QTuioCursor&);

    /**
     * Removes cursor referenced by given id.
     * Returns true if removal successful, false otherwise.
    */
    bool removeCursor(int id);

signals:
    void cursorChanged(int id, TuioCursorTableModel::CursorChange change);

public slots:
    /**
     * Handles bulk update of model.
     * Adds or updates all cursors in active_cursors and
     * deletes all cursors in dead_cursors if contained by model.
    */
    void onCursorEvent(const QMap<int, QTuioCursor>& active_cursors,
                       const QVector<QTuioCursor>& dead_cursors);

protected:
    /**
     * Helper function to access properties of QTuioCursor by field number.
    */
    QVariant data(const QTuioCursor& cursor, int field) const;

    /**
     * Helper function to access names of QTuioCursor fields by field number.
    */
    QVariant horizontalHeaderData(int field) const;

    /**
     * Internal function to add cursor to the model.
     * emits calls to update layout, etc.
     * Assumes prior validation of operation.
    */
    void internalAddCursor(const QTuioCursor& c);

    /**
     * Internal function to add cursor to the model.
     * emits calls to update layout, etc.
     * Assumes prior validation of operation.
    */
    void internalUpdateCursor(const QTuioCursor& c);

    /**
     * Internal function to remove cursor from the model.
     * emits calls to update layout, etc.
     * Assumes prior validation of operation.
    */
    void internalRemoveCursor(int id);

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

    QMap<int, QTuioCursor> cursors_;

};

#endif // TUIO_CURSOR_TABLE_MODEL_H
