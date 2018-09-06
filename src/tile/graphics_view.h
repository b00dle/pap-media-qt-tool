#ifndef TILE_GRAPHICS_VIEW_H
#define TILE_GRAPHICS_VIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QJsonObject>
#include <QUuid>
#include <QStack>
#include <QMenu>

#include "db/model/sound_file_table_model.h"
#include "db/model/preset_table_model.h"
#include "base_tile.h"
#include "image/view.h"
#include "image/image_display_widget.h"
#include "nested_path_widget.h"

// TODO: rename namespace to Tile
namespace Tile {

/**
 * Custom QGraphicsView.
 * Designed for advanced handling of Tile (see Tile.h).
 * Evaulates drops, instanciating derived Tile objects.
 * Implements bahavior for adapting screen size to widget resize.
 * Implements forwarding of drops to colliding Tile instances.
 * Holds functionality to convert all tiles in scene to JSON description
 * and be set from JSON.
*/
class GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    GraphicsView(QGraphicsScene *scene, QWidget *parent);
    GraphicsView(QWidget *parent = 0);
    ~GraphicsView();

    /**
     * Serializes this graphics view and all layouts defined for it.
     * If exclude layouts is set true, only the current view will be serialized.
    */
    const QJsonObject toJsonObject(bool exclude_layouts = false) const;

    /**
     * Creates all tiles in scene from JSON object.
     * Deletes existing scene.
     * Clears all layouts and adds all read from JSON.
     * Returns success of parsing.
    */
    bool setFromJsonObject(const QJsonObject& obj);

    void setSoundFileModel(DB::Model::SoundFileTableModel* m);
    DB::Model::SoundFileTableModel* getSoundFileModel();

    void setPresetModel(DB::Model::PresetTableModel* m);
    DB::Model::PresetTableModel* getPresetModel();

    void clear();

    /**
     * Returns the tile with given uuid or 0 if doesn't exist.
    */
    BaseTile* getTile(const QUuid& uuid) const;

    /**
     * Returns all tiles with the alternative selection falg set to true.
     * (See BaseTile::is_selected_).
    */
    QList<BaseTile*> const getSelectedTiles() const;

    /**
     * Deselects all tiles.
    */
    void deselectAllTiles();

    /**
     * Activates tile with given ID.
     * Returns true if activation succeeded.
    */
    bool activate(const QUuid& tile_id);

    /**
     * Deactivates tile with given ID.
     * Returns true if deactivation succeeded.
    */
    bool deactivate(const QUuid& tile_id);

    /**
     * Returns true if tile with given ID
     * is currently activated.
    */
    bool isActivated(const QUuid &tile_id);

    /**
     * @brief returns volume of tile or -1 if tile not found
     * @param tile_id: id of tile to receive volume of.
     * @return volume of tile.
     */
    int getVolume(const QUuid &tile_id) const;

    /**
     * @brief set volume of tile (range 0-100)
     * @param tile_id: id of tile to receive volume of.
     * @return success of setting volume.
     */
    bool setVolume(const QUuid& tile_id, int volume);

    void setImageDisplay(ImageDisplayWidget* view);

    ImageDisplayWidget* getImageDisplay() const;

    /**
     * pushes a scene onto the scene stack and shows it.
    */
    void pushScene(QGraphicsScene* s, QString const& name);

    /*
     * pops a scene from the scene stack and shows the next.
     * will not pop if there is only one scene left.
     * last scene on stakc is always main scene.
    */
    void popScene();

    /**
     * Get context menu of view.
    */
    const QMenu* getContextMenu() const;

    /**
     * Creates an empty PlaylistTile
    */
    BaseTile* createEmptyPlaylistTile(QPoint const& p);

    /**
     * Creates an empty NestedTile
    */
    BaseTile* createEmptyNestedTile(QPoint const& p);

    /*
     * @brief Creates an empty SpotifyTile
     * @param p
     */
    BaseTile* createEmptySpotifyTile(QPoint const& p);

    /*
     * @brief Creates an empty MapTile
     * @param p
     */
    BaseTile* createEmptyMapTile(QPoint const& p);

    /**
     * returns true if this instance manages a layout with given name.
    */
    bool hasLayout(const QString& name) const;

    /**
     * Saves the current view as a layout with given name.
    */
    void storeAsLayout(QString const& name);

    /**
     * Saves given JsonObject as a layout.
     * Function will NOT check if object is well-formed
     * with respect to layout json formatting.
    */
    void storeAsLayout(QString const& name, const QJsonObject& layout);

    /**
     * Loads the layout specified by given name.
     * Returns false if no layout exists with given name,
     * or layout couldn't be parsed.
    */
    bool loadLayout(QString const& name);

    /*
     * Removes given layout from the map of layouts managed by this instance.
    */
    void removeLayout(QString const& name);

    /**
     * Removes all layouts managed by this instance.
    */
    void clearLayouts();

    /**
     * Returns the names for all layouts defined on this instance.
    */
    const QStringList getLayoutNames() const;

    /**
     * Returns tile at given pos or nullptr if none there.
    */
    BaseTile* getTileAt(const QPoint& pos) const;

private:
    /**
     * Handle scene size when widget resizes.
     * Scene extends with bigger widget size.
     * Scene never shrinks.
     * (so items placed in scene bounds can not get lost without moving).
    */
    void resizeEvent(QResizeEvent* e);

    void wheelEvent(QWheelEvent *event);

signals:
    void dropAccepted();
    void layoutAdded(const QString& name);

private slots:
    void onEmptyPlaylistTile();
    void onEmptyNestedTile();
    void onEmptySpotifyTile();
    void onEmptyMapTile();
    void onNestSelectedTiles();

private:
    /**
     * Helper function for loadLayout to ensure
     * That contents and settings of tile are
     * loaded for respective UUID.
     * -> mobile app uses simpler tile description.
    */
    const QJsonObject sanitizeLayout(const QJsonObject&) const;

    /**
     * accept drags.
    */
    void dragEnterEvent(QDragEnterEvent *event);

    /**
     * accept move.
    */
    void dragMoveEvent(QDragMoveEvent *event);

    /**
    * Derived tile types get deleted on drop into empty area of scene.
    * Drops are forwarded to Tile containing mouse position.
    */
    void dropEvent(QDropEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    /**
     * Returns a string with html formatting
     * to encapsulate the current scene stack.
    */
    QString const getScenePathHTML() const;

    /**
     * Remove all tiles from view.
     */
    void clearTiles();

    /**
     * @brief initializes the context menu of this view.
     */
    void initContextMenu();

    /**
     * initializes path widget.
    */
    void initWidgets();

    DB::Model::SoundFileTableModel* sound_model_;
    DB::Model::PresetTableModel* preset_model_;
    QGraphicsScene* main_scene_;
    QStack<QGraphicsScene*> scene_stack_;
    QMap<QGraphicsScene*, QString> scene_names_;
    QMenu* context_menu_;
    QAction* nest_selected_action_;
    QPoint click_pos_;
    QMap<QString, QJsonObject> layouts_;
    Image::View* image_view_;
    ImageDisplayWidget* image_widget_;
    NestedPathWidget* nested_path_widget_;
};

} // namespace Tile

#endif // TILE_GRAPHICS_VIEW_H
