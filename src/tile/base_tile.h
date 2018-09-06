#ifndef TWO_D_TILE_H
#define TWO_D_TILE_H

#include <QObject>
#include <QGraphicsItem>
#include <QElapsedTimer>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsSceneHoverEvent>
#include <QTimer>
#include <QLineF>
#include <QMouseEvent>
#include <QMediaPlayer>
#include <QShortcut>
#include <QJsonObject>
#include <QUuid>

#include "db/handler.h"
#include "db/model/preset_table_model.h"
#include "tracking/trackable.h"

namespace Tile {

/**
 * Square 2D tile.
 * Supports hover, onlick and drag handling.
 * Uses layout mechanism, so Multiple instances of the class cannot visually overlap.
 * Defines inteface for evaluating mime data and Setting activation shortscuts.
 * Holds functionality to convert to JSON description and be set from JSON.
*/
class BaseTile : public QObject, public QGraphicsItem, public Trackable
{
    Q_OBJECT

    Q_INTERFACES(QGraphicsItem) // so instances can be casted using qobject_cast
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(qreal size MEMBER size_ READ getSize WRITE setSize)

protected:
    /**
     * state of the graphics item
    */
    enum ItemMode {
        IDLE,
        HOVER,
        ACTIVATED,
        MOVE
    };

    /**
     *  side of a 2d box
    */
    enum BOX_SIDE {
        LEFT,
        RIGHT,
        UPPER,
        LOWER
    };

public:
    BaseTile(QGraphicsItem* parent = 0);
    ~BaseTile();

    /**
     * See BC.
    */
    virtual QRectF boundingRect() const;

    /**
     * See BC.
    */
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    /** See BC. */
    virtual const QList<int> supportedTargetProperties() const;

    /** See BC. */
    virtual bool updateGrabFromTracker(Tracker *tracker, int target_prop);

    /** See BC. */
    virtual bool updateLinkFromTracker(Tracker *tracker, int target_prop);

    /** See BC. */
    virtual void setTrackableName(const QString& name);

    /**
     * Initialize default properties of tile.
     * createContextMenu will be called by this function.
    */
    virtual void init();

    /**
     * Set key for quick activate.
    */
    void setActivateKey(const QChar& c);

    /**
     * Get key for quick activate.
    */
    const QChar& getActivateKey() const;

    /**
     * Set size of tile.
     * (will not preserve non overlapping state with other tiles)
    */
    virtual void setSize(qreal size);

    /**
     * Get size of tile
    */
    virtual qreal getSize() const;

    /**
     * Animated change of tile size.
     * (will preserve non overlapping state with other tiles)
    */
    virtual void setSizeAnimated(qreal size, int duration = 300);

    /**
     * Change size of tile taking into account any overlapping with other Tiles.
    */
    virtual void setSizeLayoutAware(qreal size);

    /**
     * Animated change of tile pos.
    */
    virtual void setPosAnimated(const QPointF& p, int duration = 300);

    /**
     * Set name of tile.
    */
    void setName(const QString& str);

    /**
     * Get name of tile.
    */
    const QString& getName() const;

    /**
     * Get context menu of tile.
    */
    const QMenu* getContextMenu() const;

    /**
    * Get uuid of tile.
    */
    const QUuid& getUuid() const;

    /**
     * Returns true if a quick activation key has been set.
    */
    bool hasActivateKey() const;

    /**
     * Hand mime data such as drop data to tile.
     * This class only prints the mime text.
     * Override for derived class behavior.
    */
    virtual void receiveExternalData(const QMimeData* data);

    virtual void receiveWheelEvent(QWheelEvent *event);

    /**
     * Returns a QJsonObject holding all information about the tile
    */
    virtual const QJsonObject toJsonObject() const;

    /**
     * Set all values held by JSON object.
     * Returns success of parsing JsonObject.
    */
    virtual bool setFromJsonObject(const QJsonObject& obj);

    /**
     * Loads the overlay image from given filepath.
    */
    void loadOverlayPixmap(const QString& file_path);

    /**
     * Returns the filepath of the current overlay image.
     * Will be empty string if default pixmap is used.
    */
    const QString& getOverlayPixmapPath() const;

    /**
    * Removes the custom set overlay pixmap and returns to default.
    * (see getOverlayPixmap())
    */
    void clearOverlayPixmap();

    /**
     * Returns true if state is active
    */
    bool isActivated() const;

    /**
     * Sets activation state with respect to given bool
    */
    void setActivated(bool state);

    /**
     * sets the presetmodel for this Tile.
     */
    void setPresetModel(DB::Model::PresetTableModel* model);

    /**
     * gets the presetmodel for this Tile.
     */
    DB::Model::PresetTableModel* getPresetModel();

    /**
     * Get alternative selection state to work around
     * default graphics item selection.
     * This is necessary because selection gets confused
     * by other on click behavior.
    */
    bool getIsSelected() const;

    /**
     * Set alternative selection state to work around
     * default graphics item selection.
     * This is necessary because selection gets confused
     * by other on click behavior.
    */
    void setIsSelected(bool state);

    /**
     * Selects or deselects tile based on current state.
    */
    void toggleSelection();

signals:
    void mousePressed(QGraphicsSceneMouseEvent* e);
    void mouseReleased(QGraphicsSceneMouseEvent* e);
    void mouseMoved(QGraphicsSceneMouseEvent* e);
    void hoverEntered(QGraphicsSceneHoverEvent *e);
    void hoverLeft(QGraphicsSceneHoverEvent *e);
    void activated();

public slots:
    /** interface for tile interaction */
    virtual void onActivate();

    /** sets small size for tile */
    virtual void setSmallSize();

    /** sets medium size for tile */
    virtual void setMediumSize();

    /** sets large size for tile */
    virtual void setLargeSize();

    /** removes this item from the scene and schedules deletion (see deleteLater) */
    virtual void onDelete();

    /** saves the tile to the preset model */
    void onSaveAsPreset();

protected slots:
    /** slot to enable move mode after timer */
    virtual void onLongClick();

    /** sets the activation key using char input dialog */
    void onSetKey();

    /** Opens a dialog to configure the current activation tracker */
    void onSetActivationTracker();

    void onTrackerAdded(QString const&);

protected:
    /*
     * BC overrides
    */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* e);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* e);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *e);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *e);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* e);
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dropEvent(QGraphicsSceneDragDropEvent *event);

    /**
     * creates a drag from this tile
    */
    virtual void performDrag();

    /** Called whenever a tracker matching trackable name has been added */
    virtual void trackableSourceAddedEvent(Tracker*);

    /**
    * relayouts all other tiles based on overlaps created by resize operation
    */
    virtual void fixOverlapsAfterResize(qreal prev_size);

    /**
    * Returns QRectF definition for draw area.
    */
    virtual const QRectF getPaintRect() const;

    /**
    * Returns QRectF definition for text area.
    */
    virtual const QRectF getTextRect() const;

    /**
    * Returns default background color brush based on ItemMode.
    */
    virtual const QBrush getBackgroundBrush() const;

    /**
    * Returns tile background overlay pixmap.
    */
    virtual const QPixmap getOverlayPixmap() const;

    /**
    * Returns activate shortcut pixamp.
    */
    const QPixmap getActivatePixmap() const;

    /**
    * Sets default opacity value based on ItemState.
    */
    virtual void setDefaultOpacity();

    /*
     * setter
    */
    void setMode(ItemMode mode);

    /**
     * Returns distance between given point and line.
    */
    static qreal distance(const QPointF& p, const QLineF& l);

    /**
     * Closest side of given box to given point.
    */
    static BOX_SIDE closestSide(const QPointF& p, const QRectF& rect);

    /**
     * Creates context menu.
    */
    virtual void createContextMenu();

    QString name_;
    QTimer* long_click_timer_;
    int long_click_duration_;
    ItemMode mode_;
    qreal size_;
    QMenu* context_menu_;
    QAction* activate_action_;
    QChar activate_key_;
    QPixmap* overlay_pixmap_;
    QString overlay_pixmap_path_;
    QUuid uuid_;
    bool is_activated_;
    DB::Model::PresetTableModel* preset_model_;
    bool is_selected_;
    bool ctrl_clicked_;
};

} // namespace Tile

#endif // TWO_D_TILE_ITEM_H
