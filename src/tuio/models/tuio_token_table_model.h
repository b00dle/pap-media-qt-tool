#ifndef TUIO_TOKEN_TABLE_MODEL_H
#define TUIO_TOKEN_TABLE_MODEL_H

#include <QAbstractTableModel>

#include "qtuiotoken_p.h"

/**
 * Table model to maintain QTuioTokens.
 * Editting through regular table views is disabled.
 * Functionality to add/update/remove tokens is provided.
 * Can be connected to QTuioHandler::tokenEvent(...)
 * (see onTokenEvent(...) slot) for bulk update and easy maintanance.
 * Use QPersistentModelIndex to keep track of QTuioTokens managed here,
 * or track desired id and lookup row using utility functions provided.
*/
class TuioTokenTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum TokenChange {
        TOKEN_ADDED,
        TOKEN_UPDATED,
        TOKEN_REMOVED
    };

public:
    explicit TuioTokenTableModel(QObject *parent = nullptr);

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
     * Returns a copy to the token referenced by given id.
     * Id of returned token will be -1 if given id
     * does not reference a token managed by this model.
    */
    const QTuioToken getToken(int id) const;

    /**
     * Returns the row for token referenced by id,
     * or -1 if no token with given id is managed by this model.
    */
    int getRow(int id) const;

    /**
     * Returns the row for given token,
     * or -1 if no token with given id is managed by this model.
    */
    int getRow(const QTuioToken& c) const;

    /**
     * Returns true if a QTuioToken with given id is maintained by this model.
     * False otherwise.
    */
    bool hasToken(int id) const;

    /**
     * Adds given token to the list of tokens maintained by the model.
     * Returns true if token added.
     * If a token with id of given token already exists, no new entry
     * will be created and false is returned.
    */
    bool addToken(const QTuioToken&);

    /**
     * Adds a placeholder QTuioToken with given id and
     * default property values into the model.
     * Returns true if token created, or false
     * if token already exists in model.
    */
    bool addToken(int id);

    /**
     * Updates the values of token in model identified by given token.id
     * with all other values contained by given token.
     * Returns true if token updated, or false if token
     * does not exist.
    */
    bool updateToken(const QTuioToken&);

    /**
     * Removes token referenced by given id.
     * Returns true if removal successful, false otherwise.
    */
    bool removeToken(int id);

public slots:
    /**
     * Handles bulk update of model.
     * Adds or updates all tokens in active_tokens and
     * deletes all tokens in dead_tokens if contained by model.
    */
    void onTokenEvent(const QMap<int, QTuioToken>& active_tokens,
                       const QVector<QTuioToken>& dead_tokens);

signals:
    void tokenChanged(int id, TuioTokenTableModel::TokenChange);

protected:
    /**
     * Helper function to access properties of QTuioToken by field number.
    */
    QVariant data(const QTuioToken& token, int field) const;

    /**
     * Helper function to access names of QTuioToken fields by field number.
    */
    QVariant horizontalHeaderData(int field) const;

    /**
     * Internal function to add token to the model.
     * emits calls to update layout, etc.
     * Assumes prior validation of operation.
    */
    void internalAddToken(const QTuioToken& t);

    /**
     * Internal function to add token to the model.
     * emits calls to update layout, etc.
     * Assumes prior validation of operation.
    */
    void internalUpdateToken(const QTuioToken& t);

    /**
     * Internal function to remove token from the model.
     * emits calls to update layout, etc.
     * Assumes prior validation of operation.
    */
    void internalRemoveToken(int id);

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

    QMap<int, QTuioToken> tokens_;
};

#endif // TUIO_TOKEN_TABLE_MODEL_H
