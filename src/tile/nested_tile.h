#ifndef TWO_D_NESTED_TILE_H
#define TWO_D_NESTED_TILE_H

#include <QGraphicsView>
#include <QTimer>
#include <QPropertyAnimation>

#include "base_tile.h"
#include "graphics_view.h"
#include "db/model/sound_file_table_model.h"

namespace Tile {

class NestedTile : public BaseTile
{
    Q_OBJECT

    Q_PROPERTY(qreal progress MEMBER progress_ READ getProgress WRITE setProgress)

public:
    NestedTile(GraphicsView* master_view, QGraphicsItem *parent=0);
    virtual ~NestedTile();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    /**
     * Parses all tiles in scene to JSON object.
    */
    const QJsonObject toJsonObject() const;

    /**
     * tile from JSON object.
    */
    bool setFromJsonObject(const QJsonObject& obj);

    void clearTiles();

    void addTiles(QList<BaseTile*> const& tiles);

    /**
     * Hand mime data such as drop data to tile.
     * This class only prints the mime text.
     * Override for derived class behavior.
    */
    virtual void receiveExternalData(const QMimeData* data);

    void setProgress(qreal v);

    qreal getProgress() const;

public slots:
    /* See BC */
    virtual void onActivate();

protected slots:
    /** slot to open contents view */
    virtual void onContents();

    /** slot to open configuration */
    virtual void onConfigure();

protected:
    /*
     * BC overrides
    */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);

    /**
     * creates context menu
    */
    virtual void createContextMenu();

    /**
     * Returns the image based on the playback state
    */
    virtual const QPixmap getPlayStatePixmap() const;

    GraphicsView* master_view_;
    QGraphicsScene* scene_;
    QTimer enter_timer_;
    qreal progress_;
    QPropertyAnimation* progress_animation_;
};

} // namespace Tile

#endif // TWO_D_NESTED_TILE_H
