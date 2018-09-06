#ifndef TILE_MAP_TILE_H
#define TILE_MAP_TILE_H

#include <QGraphicsView>

#include "base_tile.h"
#include "graphics_view.h"
#include "image/image_display_widget.h"

#include <QJsonObject>

namespace Tile {

class MapTile : public BaseTile
{
    Q_OBJECT

public:
    MapTile(QGraphicsItem *parent=0);
    virtual ~MapTile();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    /**
     * Parses all tiles in scene to JSON object.
    */
    const QJsonObject toJsonObject() const;

    /**
     * tile from JSON object.
    */
    bool setFromJsonObject(const QJsonObject& obj);

    void setMapDescription(const QJsonObject& obj);
    const QJsonObject& getMapDescription() const;

    void setImageDisplay(ImageDisplayWidget* view);
    ImageDisplayWidget* getDisplayWidget() const;

public slots:
    /* See BC */
    virtual void onActivate();

protected slots:
    void onConfigure();
    void saveChanges();

protected:

    /*
     * BC overrides
    */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);

    /**
     * creates context menu
    */
    virtual void createContextMenu();

    /**
     * Returns the image based on the playback state
    */
    virtual const QPixmap getPlayStatePixmap() const;

    QJsonObject map_description_;
    ImageDisplayWidget* display_widget_;
    InteractiveImage* image_;
};

} // namespace Tile

#endif // TILE_MAP_TILE_H
