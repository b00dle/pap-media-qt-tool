#include "nested_tile.h"
#include "playlist_tile.h"

#include <QAction>
#include <QMenu>
#include <QLayout>
#include <QJsonArray>
#include <QInputDialog>

#include "resources/lib.h"

namespace Tile {

NestedTile::NestedTile(GraphicsView* master_view, QGraphicsItem *parent)
    : BaseTile(parent)
    , master_view_(master_view)
    , scene_(0)
    , enter_timer_()
    , progress_(-1.0)
    , progress_animation_(nullptr)
{
    scene_ = new QGraphicsScene(QRectF(0,0,100,100), this);
    enter_timer_.setSingleShot(true);
    connect(&enter_timer_, &QTimer::timeout,
            this, &NestedTile::onContents);
}

NestedTile::~NestedTile()
{
    clearTiles();
}

void NestedTile::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    BaseTile::paint(painter, option, widget);

    QRectF p_rect(getPaintRect());
    if(p_rect.width() > 0 && p_rect.height() > 0) {
        painter->drawPixmap(
                    (int) p_rect.x(),
                    (int) p_rect.y(),
                    (int) p_rect.width(),
                    (int) p_rect.height(),
                    *Resources::Lib::PX_FOLDER
                    );
        painter->drawPixmap(
                    (int) p_rect.x(),
                    (int) p_rect.y(),
                    (int) p_rect.width(),
                    (int) p_rect.height(),
                    getPlayStatePixmap()
                    );
    }
    if(progress_ > 0.0 && progress_ < 100.0) {
        QRectF progress_rect;
        QPointF min(
            p_rect.x() + p_rect.width() / 2.0f - (p_rect.width() / 2.0f) * (progress_ / 100.0f),
            p_rect.y() + p_rect.height() / 2.0f - (p_rect.height() / 2.0f) * (progress_ / 100.0f)
        );
        QPointF max(
            p_rect.x() + p_rect.width() / 2.0f + (p_rect.width() / 2.0f) * (progress_ / 100.0f),
            p_rect.y() + p_rect.height() / 2.0f + (p_rect.height() / 2.0f) * (progress_ / 100.0f)
        );
        progress_rect.setTopLeft(min);
        progress_rect.setBottomRight(max);
        painter->fillRect(progress_rect, QBrush(QColor(0,255,0,120)));
    }
}

const QJsonObject NestedTile::toJsonObject() const
{
    QJsonObject obj = BaseTile::toJsonObject();

    QJsonObject contents_obj;

    // parse scene properties
    QJsonObject scene_obj;
    QJsonObject scene_rect_obj;
    scene_rect_obj["x"] = scene_->sceneRect().x();
    scene_rect_obj["y"] = scene_->sceneRect().y();
    scene_rect_obj["width"] = scene_->sceneRect().width();
    scene_rect_obj["height"] = scene_->sceneRect().height();
    scene_obj["scene_rect"] = scene_rect_obj;

    // parse all tiles in scene
    QJsonArray arr_tiles;
    foreach(QGraphicsItem* it, scene_->items()) {
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

    contents_obj["scene"] = scene_obj;
    obj["contents"] = contents_obj;

    return obj;
}

bool NestedTile::setFromJsonObject(const QJsonObject &obj)
{
    if(!BaseTile::setFromJsonObject(obj))
        return false;

    if(!(obj.contains("contents") && obj["contents"].isObject()))
        return false;

    QJsonObject contents_obj = obj["contents"].toObject();
    if(contents_obj.isEmpty() || !contents_obj.contains("scene"))
        return false;
    if(!contents_obj["scene"].isObject())
        return false;

    QJsonObject sc_obj = contents_obj["scene"].toObject();
    if(!sc_obj.contains("scene_rect") || !sc_obj["scene_rect"].isObject())
        return false;
    if(!sc_obj.contains("tiles") || !sc_obj["tiles"].isArray())
        return false;

    // scene rect
    QJsonObject rc_obj = sc_obj["scene_rect"].toObject();
    if(rc_obj.contains("x") && rc_obj.contains("y") && rc_obj.contains("width") && rc_obj.contains("height")) {
        QRectF scene_rect = scene_->sceneRect();
        scene_rect.setX((qreal) rc_obj["x"].toDouble());
        scene_rect.setY((qreal) rc_obj["y"].toDouble());
        scene_rect.setWidth((qreal) rc_obj["width"].toDouble());
        scene_rect.setHeight((qreal) rc_obj["height"].toDouble());
        scene_->setSceneRect(scene_rect);
    }

    // clear tiles
    clearTiles();

    QString pl_class = PlaylistTile::staticMetaObject.className();
    QString nested_class = NestedTile::staticMetaObject.className();

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
            tile->setSoundFileModel(master_view_->getSoundFileModel());
            tile->setFlag(QGraphicsItem::ItemIsMovable, true);
            tile->init();
            if(tile->setFromJsonObject(t_obj["data"].toObject())) {
                scene_->addItem(tile);
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
            NestedTile* tile = new NestedTile(master_view_);
            tile->setFlag(QGraphicsItem::ItemIsMovable, true);
            tile->init();
            if(tile->setFromJsonObject(t_obj["data"].toObject())) {
                scene_->addItem(tile);
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

    return true;
}

void NestedTile::clearTiles()
{
    foreach(QGraphicsItem* it, scene_->items()) {
        QObject* o = dynamic_cast<QObject*>(it);
        if(o) {
            BaseTile* t = qobject_cast<BaseTile*>(o);
            t->onDelete();
        }
    }
}

void NestedTile::addTiles(const QList<BaseTile *> &tiles)
{
    // TODO: ensure layout
    foreach(auto t, tiles) {
        if(t == this)
            continue;
        scene_->addItem(t);
    }
}

void NestedTile::receiveExternalData(const QMimeData *data)
{
    Q_UNUSED(data);
    enter_timer_.stop();
    if(progress_animation_) {
        progress_animation_->stop();
        progress_animation_->deleteLater();
        progress_animation_ = nullptr;
        setProgress(-1.0);
    }
}

void NestedTile::setProgress(qreal v)
{
    prepareGeometryChange();
    progress_ = v;
}

qreal NestedTile::getProgress() const
{
    return progress_;
}

void NestedTile::onActivate()
{
    foreach(QGraphicsItem* it, scene_->items()) {
        QObject *obj = dynamic_cast<QObject*>(it);
        if(obj) {
            BaseTile* t = qobject_cast<BaseTile*>(obj);
            t->onActivate();
        }
    }

    BaseTile::onActivate();
}

void NestedTile::onContents()
{
    master_view_->pushScene(scene_, getName());
    enter_timer_.stop();
    if(progress_animation_) {
        progress_animation_->stop();
        progress_animation_->deleteLater();
        progress_animation_ = nullptr;
        setProgress(-1);
    }
}

void NestedTile::onConfigure()
{
    bool ok;
    QString text = QInputDialog::getText(
                0, tr("Set Name"),
                tr("Name:"), QLineEdit::Normal,
                name_, &ok
                );
    if (ok && !text.isEmpty())
        name_ = text;
}

void NestedTile::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    if(mode_ != MOVE && e->button() == Qt::LeftButton) {
        if(e->modifiers() & Qt::ControlModifier)
            return BaseTile::mouseReleaseEvent(e);
        onActivate();
    }
    BaseTile::mouseReleaseEvent(e);
}

void NestedTile::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    BaseTile::dragEnterEvent(event);
    enter_timer_.start(1000);
    if(progress_animation_ == nullptr) {
        progress_animation_ = new QPropertyAnimation(this, "progress");
        progress_animation_->setStartValue(0.1);
        progress_animation_->setEndValue(100.0);
        progress_animation_->setDuration(1000);
        progress_animation_->setEasingCurve(QEasingCurve::Linear);
        progress_animation_->start();
    }
}

void NestedTile::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    BaseTile::dragLeaveEvent(event);
    enter_timer_.stop();
    if(progress_animation_) {
        progress_animation_->stop();
        progress_animation_->deleteLater();
        progress_animation_ = nullptr;
        setProgress(-1);
    }
}

void NestedTile::createContextMenu()
{
    QAction* contents_action = new QAction(tr("Contents..."),this);

    connect(contents_action, SIGNAL(triggered()),
            this, SLOT(onContents()));

    QAction* configure_action = new QAction(tr("Configure..."),this);

    connect(configure_action, SIGNAL(triggered()),
            this, SLOT(onConfigure()));

    context_menu_->addAction(contents_action);
    context_menu_->addAction(configure_action);
    context_menu_->addSeparator();

    BaseTile::createContextMenu();
}

const QPixmap NestedTile::getPlayStatePixmap() const
{
    if(is_activated_)
        return *Resources::Lib::PX_STOP;
    else
        return *Resources::Lib::PX_PLAY;
}

}
