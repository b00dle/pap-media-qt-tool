#include "graphics_view.h"

#include <QCoreApplication>
#include <QDebug>
#include <QJsonArray>
#include <QMimeData>
#include <QJsonValue>
#include <QMessageBox>
#include <QHBoxLayout>

#include "playlist_tile.h"
#include "nested_tile.h"
#include "spotify_tile.h"
#include "map_tile.h"
#include "misc/json_mime_data_parser.h"
#include "resources/lib.h"

namespace Tile {

GraphicsView::GraphicsView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
    , sound_model_(0)
    , main_scene_(scene)
    , scene_stack_()
    , scene_names_()
    , context_menu_(0)
    , nest_selected_action_(0)
    , click_pos_()
    , layouts_()
    , image_widget_(0)
    , nested_path_widget_(0)
{
    pushScene(main_scene_, "MAIN");
    setAcceptDrops(true);
    setFocusPolicy(Qt::ClickFocus);
    initWidgets();
    initContextMenu();
}

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView(parent)
    , sound_model_(0)
    , main_scene_(0)
    , scene_stack_()
    , scene_names_()
    , context_menu_(0)
    , nest_selected_action_(0)
    , click_pos_()
    , layouts_()
    , image_widget_(0)
    , nested_path_widget_(0)
{
    main_scene_ = new QGraphicsScene(QRectF(0,0,100,100),this);
    pushScene(main_scene_, "MAIN");
    setAcceptDrops(true);
    setFocusPolicy(Qt::ClickFocus);
    initWidgets();
    initContextMenu();
}

GraphicsView::~GraphicsView()
{
    context_menu_->deleteLater();
    if(scene())
        scene()->clear();
}

const QJsonObject GraphicsView::toJsonObject(bool exclude_layouts) const
{
    QJsonObject obj;

    // parse scene properties
    QJsonObject scene_obj;
    QJsonObject scene_rect_obj;
    scene_rect_obj["x"] = sceneRect().x();
    scene_rect_obj["y"] = sceneRect().y();
    scene_rect_obj["width"] = sceneRect().width();
    scene_rect_obj["height"] = sceneRect().height();
    scene_obj["scene_rect"] = scene_rect_obj;

    // parse all tiles in scene
    QJsonArray arr_tiles;
    foreach(QGraphicsItem* it, scene()->items()) {
        QObject *obj = dynamic_cast<QObject*>(it);
        if(obj) {
            BaseTile* t = qobject_cast<BaseTile*>(obj);
            QJsonObject obj_tile;
            obj_tile["type"] = QJsonValue(t->metaObject()->className());
            obj_tile["data"] = QJsonValue(t->toJsonObject());
            arr_tiles.append(obj_tile);
        }
    }
    scene_obj["tiles"] = QJsonValue(arr_tiles);

    obj["scene"] = scene_obj;

    if(!exclude_layouts) {
        QJsonObject obj_l;
        foreach(auto l_str, layouts_.keys())
            obj_l[l_str] = QJsonValue(layouts_[l_str]);
        obj["layouts"] = obj_l;
    }

    return obj;
}

bool GraphicsView::setFromJsonObject(const QJsonObject &obj)
{
    if(obj.isEmpty() || !obj.contains("scene"))
        return false;
    if(!obj["scene"].isObject())
        return false;

    QJsonObject sc_obj = obj["scene"].toObject();
    if(!sc_obj.contains("scene_rect") || !sc_obj["scene_rect"].isObject())
        return false;
    if(!sc_obj.contains("tiles") || !sc_obj["tiles"].isArray())
        return false;

    clear();

    // scene rect
    QJsonObject rc_obj = sc_obj["scene_rect"].toObject();
    if(rc_obj.contains("x") && rc_obj.contains("y") && rc_obj.contains("width") && rc_obj.contains("height")) {
        QRectF scene_rect = sceneRect();
        scene_rect.setX((qreal) rc_obj["x"].toDouble());
        scene_rect.setY((qreal) rc_obj["y"].toDouble());
        scene_rect.setWidth((qreal) rc_obj["width"].toDouble());
        scene_rect.setHeight((qreal) rc_obj["height"].toDouble());
        scene()->setSceneRect(scene_rect);
    }

    QString pl_class = PlaylistTile::staticMetaObject.className();
    QString nested_class = NestedTile::staticMetaObject.className();
    QString spotify_class = SpotifyTile::staticMetaObject.className();
    QString map_class = MapTile::staticMetaObject.className();

    // tiles
    QJsonArray arr_tiles = sc_obj["tiles"].toArray();
    foreach(QJsonValue val, arr_tiles) {
        if(!val.isObject())
            continue;
        QJsonObject t_obj = val.toObject();
        if(!t_obj.contains("type") || !t_obj.contains("data") || !t_obj["data"].isObject())
            continue;

        // create tile, if type is Tile::PlaylistTile
        if(t_obj["type"].toString().compare(pl_class) == 0) {
            PlaylistTile* tile = new PlaylistTile;
            tile->setSoundFileModel(sound_model_);
            tile->setPresetModel(preset_model_);
            tile->setFlag(QGraphicsItem::ItemIsMovable, true);
            tile->init();
            if(tile->setFromJsonObject(t_obj["data"].toObject())) {
                scene()->addItem(tile);
            }
            else {
                qDebug() << "FAILURE: Could not set Tile data from JSON.";
                qDebug() << " > data:" << t_obj["data"];
                qDebug() << " > Aborting.";
                delete tile;
                return false;
            }
        }
        else if(t_obj["type"].toString().compare(nested_class) == 0) {
            NestedTile* tile = new NestedTile(this);
            tile->setPresetModel(preset_model_);
            tile->setFlag(QGraphicsItem::ItemIsMovable, true);
            tile->init();
            if(tile->setFromJsonObject(t_obj["data"].toObject())) {
                scene()->addItem(tile);
            }
            else {
                qDebug() << "FAILURE: Could not set Tile data from JSON.";
                qDebug() << " > data:" << t_obj["data"];
                qDebug() << " > Aborting.";
                delete tile;
                return false;
            }
        }
        else if(t_obj["type"].toString().compare(spotify_class) == 0) {
            SpotifyTile* tile = new SpotifyTile;
            tile->setFlag(QGraphicsItem::ItemIsMovable, true);
            tile->setPresetModel(preset_model_);
            tile->init();
            if(tile->setFromJsonObject(t_obj["data"].toObject())) {
                scene()->addItem(tile);
            }
            else {
                qDebug() << "FAILURE: Could not set Tile data from JSON.";
                qDebug() << " > data:" << t_obj["data"];
                qDebug() << " > Aborting.";
                delete tile;
                return false;
            }
        }
        else if(t_obj["type"].toString().compare(map_class) == 0) {
            MapTile* tile = new MapTile;
            tile->setFlag(QGraphicsItem::ItemIsMovable, true);
            tile->setPresetModel(preset_model_);
            tile->setImageDisplay(image_widget_);
            tile->init();
            if(tile->setFromJsonObject(t_obj["data"].toObject())) {
                scene()->addItem(tile);
            }
            else {
                qDebug() << "FAILURE: Could not set Tile data from JSON.";
                qDebug() << " > data:" << t_obj["data"];
                qDebug() << " > Aborting.";
                delete tile;
                return false;
            }
        }
    }

    if(obj.contains("layouts") && obj["layouts"].isObject()) {
        layouts_.clear();
        QJsonObject l_obj = obj["layouts"].toObject();
        foreach(auto l_key, l_obj.keys()) {
            if(!l_obj[l_key].isObject())
                continue;
            layouts_[l_key] = l_obj[l_key].toObject();
        }
    }

    return true;
}

void GraphicsView::setSoundFileModel(DB::Model::SoundFileTableModel *m)
{
    sound_model_ = m;
}

DB::Model::SoundFileTableModel *GraphicsView::getSoundFileModel()
{
    return sound_model_;
}

void GraphicsView::setPresetModel(DB::Model::PresetTableModel *m)
{
    preset_model_ = m;
}

DB::Model::PresetTableModel *GraphicsView::getPresetModel()
{
    return preset_model_;
}

void GraphicsView::clear()
{
    if(!scene())
        return;
    while(scene() != main_scene_)
        popScene();
    clearTiles();
    scene()->clear();
}

BaseTile *GraphicsView::getTile(const QUuid &uuid) const
{
    foreach(QGraphicsItem* it, scene()->items()) {
        QObject* o = dynamic_cast<QObject*>(it);
        if(o) {
            BaseTile* t = qobject_cast<BaseTile*>(o);
            if(t && t->getUuid() == uuid) {
                return t;
            }
        }
    }
    return 0;
}

const QList<BaseTile *> GraphicsView::getSelectedTiles() const
{
    QList<BaseTile*> selected_tiles;
    foreach(auto it, items()) {
        BaseTile* tile = qgraphicsitem_cast<BaseTile*>(it);
        if(tile && tile->getIsSelected()) {
            selected_tiles.append(tile);
        }
    }
    return selected_tiles;
}

void GraphicsView::deselectAllTiles()
{
    foreach(auto t, getSelectedTiles())
        t->setIsSelected(false);
}

bool GraphicsView::activate(const QUuid &tile_id)
{
    foreach(QGraphicsItem* it, scene()->items()) {
        QObject* o = dynamic_cast<QObject*>(it);
        if(o) {
            BaseTile* t = qobject_cast<BaseTile*>(o);
            if(t && t->getUuid() == tile_id && !t->isActivated()) {
                t->onActivate();
                return true;
            }
        }
    }
    return false;
}

bool GraphicsView::deactivate(const QUuid &tile_id)
{
    foreach(QGraphicsItem* it, scene()->items()) {
        QObject* o = dynamic_cast<QObject*>(it);
        if(o) {
            BaseTile* t = qobject_cast<BaseTile*>(o);
            if(t && t->getUuid() == tile_id && t->isActivated()) {
                t->onActivate();
                return true;
            }
        }
    }
    return false;
}

bool GraphicsView::isActivated(const QUuid &tile_id)
{
    foreach(QGraphicsItem* it, scene()->items()) {
        QObject* o = dynamic_cast<QObject*>(it);
        if(o) {
            BaseTile* t = qobject_cast<BaseTile*>(o);
            if(t && t->getUuid() == tile_id && t->isActivated()) {
                return t->isActivated();
            }
        }
    }
    return false;
}

int GraphicsView::getVolume(const QUuid &tile_id) const
{
    QString playlist_class = Tile::PlaylistTile::staticMetaObject.className();
    foreach(QGraphicsItem* it, scene()->items()) {
        QObject* o = dynamic_cast<QObject*>(it);
        if(o) {
            BaseTile* t = qobject_cast<BaseTile*>(o);
            if(t && t->getUuid() == tile_id && t->isActivated()) {
                QString t_class(t->metaObject()->className());
                if(t_class.compare(playlist_class) == 0) {
                    PlaylistTile* p = (PlaylistTile*) t;
                    return p->getVolume();
                }
            }
        }
    }
    return -1;
}

bool GraphicsView::setVolume(const QUuid &tile_id, int volume)
{
    QString playlist_class = Tile::PlaylistTile::staticMetaObject.className();
    foreach(QGraphicsItem* it, scene()->items()) {
        QObject* o = dynamic_cast<QObject*>(it);
        if(o) {
            BaseTile* t = qobject_cast<BaseTile*>(o);
            if(t && t->getUuid() == tile_id && t->isActivated()) {
                QString t_class(t->metaObject()->className());
                if(t_class.compare(playlist_class) == 0) {
                    PlaylistTile* p = (PlaylistTile*) t;
                    p->setVolume(volume);
                    return true;
                }
            }
        }
    }
    return false;
}

void GraphicsView::setImageDisplay(ImageDisplayWidget *image_display)
{
    image_widget_ = image_display;
}

ImageDisplayWidget *GraphicsView::getImageDisplay() const
{
    return image_widget_;
}

void GraphicsView::pushScene(QGraphicsScene* scene, QString const& name)
{
    scene_stack_.push(scene);
    scene->setSceneRect(main_scene_->sceneRect());
    scene_names_[scene] = name;
    setScene(scene);
    if(scene_stack_.size() > 1) {
        nested_path_widget_->setPathText(scene_stack_, scene_names_);
        nested_path_widget_->show();
    }
}

void GraphicsView::popScene()
{
    if(scene_stack_.size() > 1) {
        scene_names_.remove(scene_stack_.pop());
        setScene(scene_stack_.top());
        nested_path_widget_->setPathText(scene_stack_, scene_names_);
        if(scene_stack_.size() == 1)
            nested_path_widget_->hide();
    }
}

const QMenu *GraphicsView::getContextMenu() const
{
    return context_menu_;
}

BaseTile* GraphicsView::createEmptyPlaylistTile(const QPoint &p)
{
    PlaylistTile* tile = new PlaylistTile;
    tile->setPresetModel(preset_model_);
    tile->setSoundFileModel(sound_model_);
    tile->setFlag(QGraphicsItem::ItemIsMovable, true);
    tile->init();
    tile->setPos(p);
    tile->setSize(0);
    tile->setName("Empty Playlist");

    // add to scene
    scene()->addItem(tile);
    tile->setSmallSize();

    return tile;
}

BaseTile* GraphicsView::createEmptyNestedTile(const QPoint &p)
{
    NestedTile* tile = new NestedTile(this);
    tile->setPresetModel(preset_model_);
    tile->setFlag(QGraphicsItem::ItemIsMovable, true);
    tile->init();
    tile->setPos(p);
    tile->setSize(0);
    tile->setName("Empty Nested");

    // add to scene
    scene()->addItem(tile);
    tile->setSmallSize();

    return tile;
}

BaseTile* GraphicsView::createEmptySpotifyTile(const QPoint &p)
{
    SpotifyTile* tile = new SpotifyTile;
    tile->setFlag(QGraphicsItem::ItemIsMovable, true);
    tile->setPresetModel(preset_model_);
    tile->init();
    tile->setPos(p);
    tile->setSize(0);
    tile->setName("Empty Spotify");

    // add to scene
    scene()->addItem(tile);
    tile->setSmallSize();

    return tile;
}

BaseTile* GraphicsView::createEmptyMapTile(const QPoint &p)
{
    MapTile* tile = new MapTile;
    tile->setFlag(QGraphicsItem::ItemIsMovable, true);
    tile->setPresetModel(preset_model_);
    tile->setImageDisplay(image_widget_);
    tile->init();
    tile->setPos(p);
    tile->setSize(0);
    tile->setName("Empty Map");

    // add to scene
    scene()->addItem(tile);
    tile->setSmallSize();

    return tile;
}

bool GraphicsView::hasLayout(const QString& name) const
{
    return layouts_.contains(name);
}

void GraphicsView::storeAsLayout(const QString &name)
{
    storeAsLayout(name, toJsonObject(true));
}

void GraphicsView::storeAsLayout(const QString &name, const QJsonObject &layout)
{
    layouts_[name] = layout;
    layoutAdded(name);
}

bool GraphicsView::loadLayout(const QString &name)
{
    if(!layouts_.contains(name))
        return false;
    if(name.compare("main") != 0) {
        if(!hasLayout("main")) {
            storeAsLayout("main");
        }
        else {
            QMessageBox b;
            b.setText(tr("If this is your main layout you might want to override it's current definition before switching to '") + name + tr("'."));
            b.setInformativeText(tr("Do you wish to override the main layout?"));
            b.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            b.setDefaultButton(QMessageBox::Yes);
            if(b.exec() == QMessageBox::Yes)
                storeAsLayout("main");
        }
    }
    return setFromJsonObject(sanitizeLayout(layouts_[name]));
}

void GraphicsView::removeLayout(const QString &name)
{
    if(layouts_.contains(name))
        layouts_.remove(name);
}

void GraphicsView::clearLayouts()
{
    layouts_.clear();
}

const QStringList GraphicsView::getLayoutNames() const
{
    return layouts_.keys();
}

BaseTile *GraphicsView::getTileAt(const QPoint &pos) const
{
    auto it = itemAt(pos);
    if(!it)
        return nullptr;
    return qgraphicsitem_cast<BaseTile*>(it);
}

void GraphicsView::resizeEvent(QResizeEvent *e)
{
    QGraphicsView::resizeEvent(e);
    if(e->isAccepted()) {
        QRectF r = scene()->sceneRect();
        if(e->size().width() > r.width()) {
            r.setWidth(e->size().width());
        }
        if(e->size().height() > r.height()) {
            r.setHeight(e->size().height());
        }
        scene()->setSceneRect(r);
    }
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
    if(!scene())
        return;

    QPointF p(mapToScene(event->pos()));

    foreach(QGraphicsItem* item, scene()->items()){
        if (item->contains(item->mapFromScene(p))){
            QObject *selected_object = dynamic_cast<QObject*>(item);
            if(selected_object)
            {
                BaseTile* t = qobject_cast<BaseTile*>(selected_object);
                if(t){
                    t->receiveWheelEvent(event);
                    return;
                }
            }
        }
    }
    QGraphicsView::wheelEvent(event);
}

void GraphicsView::onEmptyPlaylistTile()
{
    createEmptyPlaylistTile(click_pos_);
}

void GraphicsView::onEmptyNestedTile()
{
    createEmptyNestedTile(click_pos_);
}

void GraphicsView::onEmptySpotifyTile()
{
    createEmptySpotifyTile(click_pos_);
}

void GraphicsView::onEmptyMapTile()
{
    createEmptyMapTile(click_pos_);
}

void GraphicsView::onNestSelectedTiles()
{
    QList<BaseTile*> selected_tiles = getSelectedTiles();
    BaseTile* new_tile = createEmptyNestedTile(click_pos_);
    NestedTile* new_nested = qgraphicsitem_cast<NestedTile*>(new_tile);
    if(new_nested) {
        int animation_duration = 300;
        QMap<BaseTile*, QPointF> prev_pos;
        QMap<BaseTile*, qreal> prev_size;
        foreach(auto tile, selected_tiles) {
            prev_pos[tile] = tile->pos();
            tile->setPosAnimated(new_nested->pos(), animation_duration);
            prev_size[tile] = tile->getSize();
            tile->setSizeAnimated(0, animation_duration);
            tile->setIsSelected(false);
        }
        QTimer::singleShot(animation_duration+50, this, [=](){
            foreach(auto tile, selected_tiles) {
                scene()->removeItem(tile);
                tile->setPos(prev_pos[tile]);
                tile->setSize(prev_size[tile]);
            }
            new_nested->addTiles(selected_tiles);
        });
    }
}

const QJsonObject GraphicsView::sanitizeLayout(const QJsonObject& obj) const
{
    if(!obj.contains("scene") || !obj["scene"].isObject())
        return obj;
    if(!obj["scene"].toObject().contains("tiles") || !obj["scene"].toObject()["tiles"].isArray())
        return obj;

    QJsonObject res_obj_scene;
    res_obj_scene["scene_rect"] = obj["scene"].toObject()["scene_rect"];

    QJsonArray tiles_arr;
    QJsonObject sanitized_tile, sanitized_data, new_data;
    sanitized_tile["data"] = QJsonValue();
    sanitized_tile["type"] = "";
    BaseTile* t = nullptr;
    foreach(auto dirty_tile, obj["scene"].toObject()["tiles"].toArray()) {
        new_data = dirty_tile.toObject()["data"].toObject();
        t = getTile(QUuid(new_data["uuid"].toString()));
        if(t) {
            sanitized_data = t->toJsonObject();
            sanitized_data["name"] = new_data["name"].toString();
            sanitized_data["position"] = new_data["position"].toArray();
            sanitized_data["size"] = new_data["size"].toInt();

            sanitized_tile["data"] = sanitized_data;
            sanitized_tile["type"] = t->metaObject()->className();

            tiles_arr.append(sanitized_tile);
        }
        else {
            tiles_arr.append(dirty_tile);
        }
    }

    res_obj_scene["tiles"] = tiles_arr;
    QJsonObject sanitized_obj(obj);
    sanitized_obj["scene"] = res_obj_scene;
    return sanitized_obj;
}

void GraphicsView::dragEnterEvent(QDragEnterEvent *event)
{
    QGraphicsView::dragEnterEvent(event);
    GraphicsView *source = qobject_cast<GraphicsView*>(event->source());
    if (source != this) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void GraphicsView::dragMoveEvent(QDragMoveEvent *event)
{
    QGraphicsView::dragMoveEvent(event);
    GraphicsView *source = qobject_cast<GraphicsView*>(event->source());
    if (source != this) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void GraphicsView::dropEvent(QDropEvent *event)
{
    if(!scene())
        return;

    QPointF p(mapToScene(event->pos()));

    foreach(QGraphicsItem* item, scene()->items()){
        if (item->contains(item->mapFromScene(p))){
            QObject *selected_object = dynamic_cast<QObject*>(item);
            if(selected_object)
            {
                BaseTile* t = qobject_cast<BaseTile*>(selected_object);
                if(t){
                    event->accept();
                    t->receiveExternalData(event->mimeData());
                    return;
                }
            }
        }
    }

    // extract DB::TableRecord from mime data
    QList<DB::TableRecord*> records = Misc::JsonMimeDataParser::toTableRecordList(event->mimeData());

    QString pl_class = PlaylistTile::staticMetaObject.className();
    QString nested_class = NestedTile::staticMetaObject.className();
    QString spotify_class = SpotifyTile::staticMetaObject.className();
    QString map_class = MapTile::staticMetaObject.className();

    // validate parsing
    if(records.size() == 0 || records[0]->index != DB::SOUND_FILE) {
        // TODO make pretty
        QJsonDocument doc = QJsonDocument::fromJson(event->mimeData()->text().toUtf8());
        if(doc.object().contains("type") && doc.object()["type"].toString().compare(nested_class) == 0) {
            NestedTile* tile = new NestedTile(this);
            tile->setPresetModel(preset_model_);
            tile->setFlag(QGraphicsItem::ItemIsMovable, true);
            tile->setFromJsonObject(doc.object()["data"].toObject());
            tile->init();
            tile->setPos(p);
            tile->setSize(0);

            // add to scene
            scene()->addItem(tile);
            tile->setSmallSize();

            // except event
            event->setDropAction(Qt::CopyAction);
            event->accept();
            emit dropAccepted();
            return;
        }
        else if(doc.object().contains("type") && doc.object()["type"].toString().compare(pl_class) == 0) {
            PlaylistTile* tile = new PlaylistTile;
            tile->setPresetModel(preset_model_);
            tile->setSoundFileModel(sound_model_);
            tile->setFlag(QGraphicsItem::ItemIsMovable, true);
            tile->setFromJsonObject(doc.object()["data"].toObject());
            tile->init();
            tile->setPos(p);
            tile->setSize(0);

            // add to scene
            scene()->addItem(tile);
            tile->setSmallSize();

            // except event
            event->setDropAction(Qt::CopyAction);
            event->accept();
            emit dropAccepted();
            return;
        }
        // TODO: beautify
        else if(doc.object().contains("type") && doc.object()["type"].toString().compare(spotify_class) == 0) {
            SpotifyTile* tile = new SpotifyTile;
            tile->setFlag(QGraphicsItem::ItemIsMovable, true);
            tile->setPresetModel(preset_model_);
            tile->setFromJsonObject(doc.object()["data"].toObject());
            tile->init();
            tile->setPos(p);
            tile->setSize(0);

            // add to scene
            scene()->addItem(tile);
            tile->setSmallSize();

            // except event
            event->setDropAction(Qt::CopyAction);
            event->accept();
            emit dropAccepted();
            return;
        }
        // TODO: beautify
        else if(doc.object().contains("type") && doc.object()["type"].toString().compare(map_class) == 0) {
            MapTile* tile = new MapTile;
            tile->setFlag(QGraphicsItem::ItemIsMovable, true);
            tile->setPresetModel(preset_model_);
            tile->setImageDisplay(image_widget_);
            tile->init();
            tile->setFromJsonObject(doc.object()["data"].toObject());
            tile->setPos(p);
            tile->setSize(0);

            // add to scene
            scene()->addItem(tile);
            tile->setSmallSize();

            // except event
            event->setDropAction(Qt::CopyAction);
            event->accept();
            emit dropAccepted();
            return;
        }
        // TODO: beautify
        else if(event->mimeData()->text().contains("spotify")) {
            SpotifyTile* tile = new SpotifyTile;
            tile->setFlag(QGraphicsItem::ItemIsMovable, true);
            tile->setPresetModel(preset_model_);
            tile->init();
            tile->setPos(p);
            tile->setSize(0);
            tile->setName("Empty Spotify");

            // add to scene
            scene()->addItem(tile);
            tile->setSmallSize();

            // except event
            event->setDropAction(Qt::CopyAction);
            event->accept();
            emit dropAccepted();

            tile->receiveExternalData(event->mimeData());
            return;
        }
        else {
            event->ignore();
            return;
        }
    }

    // create graphics item
    PlaylistTile* tile = new PlaylistTile;
    tile->setSoundFileModel(sound_model_);
    tile->setPresetModel(preset_model_);
    tile->setFlag(QGraphicsItem::ItemIsMovable, true);
    tile->setName(records[0]->name);
    tile->init();
    tile->setPos(p);
    tile->setSize(0);

    foreach(DB::TableRecord* rec, records) {
        if(rec->index == DB::SOUND_FILE)
            tile->addMedia(*((DB::SoundFileRecord*) rec));
    }

    // add to scene
    scene()->addItem(tile);
    tile->setSmallSize();

    // except event
    event->setDropAction(Qt::CopyAction);
    event->accept();
    emit dropAccepted();

    // delete temp records
    while(records.size() > 0) {
        delete records[0];
        records.pop_front();
    }
}

void GraphicsView::keyPressEvent(QKeyEvent*)
{
    //qDebug() << event->key();
}

void GraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Backspace) {
        popScene();
        return;
    }

    foreach(QGraphicsItem* it, scene()->items()) {
        QObject* o = dynamic_cast<QObject*>(it);
        if(o) {
            BaseTile* t = qobject_cast<BaseTile*>(o);
            if(t->hasActivateKey() && t->getActivateKey() == QChar(event->key()))
                t->onActivate();
        }
    }
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    if(!event->isAccepted()) {
        if(event->button() == Qt::RightButton) {
            nest_selected_action_->setEnabled(getSelectedTiles().size() > 0);
            context_menu_->popup(event->globalPos());
            click_pos_ = event->pos();
            event->accept();
        }
    }
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && !(event->modifiers() & Qt::ControlModifier)) {
        auto tile = getTileAt(event->pos());
        if(!tile)
            deselectAllTiles();
    }
    QGraphicsView::mouseReleaseEvent(event);
}

const QString GraphicsView::getScenePathHTML() const
{
    QString path = "";
    int i = 0;
    foreach(auto s, scene_stack_) {
        if(i != 0)
            path += " > ";
        if(i == scene_stack_.size() - 1)
            path += "<b>";
        path += scene_names_[s];
        if(i == scene_stack_.size() - 1)
            path += "</b>";
        ++i;
    }
    return path;
}

void GraphicsView::clearTiles()
{
    foreach(QGraphicsItem* it, scene()->items()) {
        QObject* o = dynamic_cast<QObject*>(it);
        if(o) {
            BaseTile* t = qobject_cast<BaseTile*>(o);
            t->onDelete();
        }
    }
}

void GraphicsView::initContextMenu()
{
    context_menu_ = new QMenu;

    QMenu* create_empty = context_menu_->addMenu(tr("Create Empty"));

    QAction* empty_playlist = new QAction(tr("Playlist Tile"));
    connect(empty_playlist, SIGNAL(triggered()),
            this, SLOT(onEmptyPlaylistTile()));

    QAction* empty_nested = new QAction(tr("Nested Tile"));
    connect(empty_nested, SIGNAL(triggered()),
            this, SLOT(onEmptyNestedTile()));

    QAction* empty_spotify = new QAction(tr("Spotify Tile"));
    connect(empty_spotify, SIGNAL(triggered()),
            this, SLOT(onEmptySpotifyTile()));

    QAction* empty_map = new QAction(tr("Map Tile"));
    connect(empty_map, SIGNAL(triggered()),
            this, SLOT(onEmptyMapTile()));

    create_empty->addAction(empty_playlist);
    create_empty->addAction(empty_nested);
    create_empty->addAction(empty_spotify);
    create_empty->addAction(empty_map);

    nest_selected_action_ = context_menu_->addAction(tr("Nest Selected Tiles..."));
    connect(nest_selected_action_, &QAction::triggered,
            this, &GraphicsView::onNestSelectedTiles);
    nest_selected_action_->setEnabled(false);
}

void GraphicsView::initWidgets()
{
    // do not add to layout to create as overlay
    nested_path_widget_ = new NestedPathWidget(this);
    nested_path_widget_->move(5,5);
    nested_path_widget_->hide();
    connect(nested_path_widget_, &NestedPathWidget::backButtonClicked,
            this, &GraphicsView::popScene);
}

} // namespace Tile
