#ifndef TRACKER_TABLE_MODEL_H
#define TRACKER_TABLE_MODEL_H

#include <QAbstractTableModel>

#include "tracking/tracker.h"

class TrackerTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TrackerTableModel(QObject *parent = nullptr);
    ~TrackerTableModel();

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
     * Returns the row for tracker referenced by name,
     * or -1 if no tracker with given name is managed by this model.
    */
    int getRow(const QString& name) const;

    /**
     * Returns the row for given tracker,
     * or -1 if no tracker with given name is managed by this model.
    */
    int getRow(const Tracker& t) const;

    /**
     * Returns a copy to the tracker referenced by given name.
     * Return value will be 0 if given name
     * does not reference a tracker managed by this model.
    */
    Tracker* getTracker(const QString& name) const;

    /**
     * Returns a list of all trackers in this model
    */
    QList<Tracker*> const getTrackers() const;

    /**
     * Returns true if a Tracker with given name is maintained by this model.
     * False otherwise.
    */
    bool hasTracker(const QString& name) const;

    /**
     * Returns true if a Tracker is maintained by this model.
     * False otherwise.
    */
    bool hasTracker(Tracker* t) const;

    /**
     * Adds given tracker to the list of trackers maintained by the model.
     * Returns true if tracker added.
     * If a tracker with name of given tracker already exists, no new entry
     * will be created and false is returned.
    */
    bool addTracker(Tracker*);

    /**
     * Updates the values of tracker in model identified by given tracker.name
     * with all other values contained by given tracker.
     * Returns true if tracker updated, or false if tracker
     * does not exist.
    */
    bool updateTracker(Tracker*);

    /**
     * Removes tracker referenced by given name.
     * Returns true if removal successful, false otherwise.
    */
    bool removeTracker(const QString& name);

    /**
     * Removes tracker.
     * Returns true if removal successful, false otherwise.
    */
    bool removeTracker(Tracker* t);

signals:
    void trackerAdded(const QString& name);
    void trackerRemoved(const QString& name);

public slots:

protected:
    /**
     * Helper function to access properties of Tracker by field number.
    */
    QVariant data(Tracker* tracker, int field) const;

    /**
     * Helper function to access names of Tracker fields by field number.
    */
    QVariant horizontalHeaderData(int field) const;

    /**
     * Internal function to add tracker to the model.
     * emits calls to update layout, etc.
     * Assumes prior validation of operation.
    */
    void internalAddTracker(Tracker* t);

    /**
     * Internal function to add tracker to the model.
     * emits calls to update layout, etc.
     * Assumes prior validation of operation.
    */
    void internalUpdateTracker(Tracker* t);

    /**
     * Internal function to remove tracker from the model.
     * emits calls to update layout, etc.
     * Assumes prior validation of operation.
    */
    void internalRemoveTracker(const QString& name);

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

    /** Formats point as string and returns it. */
    static const QString toString(const QPointF& p);

    QMap<QString, Tracker*> trackers_;
};

#endif // TRACKER_TABLE_MODEL_H
