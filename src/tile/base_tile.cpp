#include "base_tile.h"

#include <QDebug>
#include <QMimeData>
#include <QGraphicsScene>
#include <QPropertyAnimation>
#include <QGraphicsPixmapItem>
#include <QMenu>
#include <QJsonArray>
#include <QErrorMessage>
#include <cmath>
#include <QJsonDocument>
#include <QMessageBox>
#include <QDrag>

#include "resources/lib.h"
#include "misc/char_input_dialog.h"
#include "tracking/activation_tracker.h"
#include "tracking/tracker_picker_dialog.h"

#define OFFSET 10
#define TEXT_HEIGHT 25
#define TEXT_POINT_SIZE 9

namespace Tile {

BaseTile::BaseTile(QGraphicsItem* parent)
    : QObject(0)
    , QGraphicsItem(parent)
    , name_()
    , long_click_timer_()
    , long_click_duration_(300)
    , mode_(IDLE)
    , size_(1)
    , context_menu_(0)
    , activate_action_(0)
    , activate_key_(' ')
    , overlay_pixmap_(0)
    , overlay_pixmap_path_()
    , uuid_()
    , is_activated_(false)
    , preset_model_(0)
    , is_selected_(false)
    , ctrl_clicked_(false)
{    
    long_click_timer_ = new QTimer(this);
    connect(long_click_timer_, SIGNAL(timeout()),
            this, SLOT(onLongClick()));
    long_click_timer_->setSingleShot(true);

    setAcceptHoverEvents(true);
    setAcceptDrops(true);

    context_menu_ = new QMenu;

    activate_action_ = new QAction("Activate", this);

    uuid_ = QUuid::createUuid();

    connect(activate_action_, SIGNAL(triggered()),
            this, SLOT(onActivate()));
}

BaseTile::~BaseTile()
{
    context_menu_->deleteLater();
    clearOverlayPixmap();
}

void BaseTile::init()
{
    createContextMenu();
}

QRectF BaseTile::boundingRect() const
{
    return QRectF(0,0,120*size_,120*size_+TEXT_HEIGHT);
}

void BaseTile::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    setDefaultOpacity();

    // paint bounding box
    QBrush b(QColor(55,55,56));
    if(is_selected_)
        b.setColor(QColor(50,152,253));
    painter->fillRect(boundingRect(), b);
    painter->setPen(QColor(84,85,86));
    painter->drawRect(boundingRect());

    // file tile
    QRectF p_rect(getPaintRect());
    painter->fillRect(p_rect, getBackgroundBrush());
    if(p_rect.width() > 0 && p_rect.height() > 0) {
        painter->setOpacity(0.6);
        painter->drawPixmap((int) p_rect.x(), (int)p_rect.y(), (int) p_rect.width(), (int) p_rect.height(), getOverlayPixmap());
        painter->setOpacity(1.0);
        QPixmap act_px = getActivatePixmap();
        if(!act_px.isNull())
            painter->drawPixmap((int) p_rect.x()+5, (int) p_rect.y()+5, (int) p_rect.width() / 4, (int) p_rect.height() / 4, act_px);
    }

    // draw name
    QFont old_font = painter->font();
    QFont font = old_font;
    /* twice the size than the current font size */
    if(font.pointSize() != TEXT_POINT_SIZE)
        font.setPointSize(TEXT_POINT_SIZE);
    /* set the modified font to the painter */
    painter->setFont(font);
    painter->setPen(QColor(Qt::white));
    painter->drawText(getTextRect(), Qt::TextWrapAnywhere | Qt::AlignCenter, name_);
    painter->setFont(old_font);
}

const QList<int> BaseTile::supportedTargetProperties() const
{
    QList<int> p;
    p.append(ActivationTracker::ACTIVE_STATE);
    return p;
}

bool BaseTile::updateGrabFromTracker(Tracker *tracker, int target_prop)
{
    Q_UNUSED(tracker);
    Q_UNUSED(target_prop);
    return false;
}

bool BaseTile::updateLinkFromTracker(Tracker *tracker, int target_prop)
{
    if(!Trackable::updateLinkFromTracker(tracker, target_prop))
        return false;
    auto act_t = dynamic_cast<ActivationTracker*>(tracker);
    if(!act_t)
        return false;
    setActivated(act_t->getActiveState());
    return true;
}

void BaseTile::setTrackableName(const QString &name)
{
    bool connect_tracker_notify = trackable_name_.size() == 0 && name.size() > 0;
    bool disconnect_tracker_notify = trackable_name_.size() > 0 && name.size() == 0;
    trackable_name_ = name;
    if(connect_tracker_notify) {
        connect(Resources::Lib::TRACKER_MODEL, &TrackerTableModel::trackerAdded,
                this, &BaseTile::onTrackerAdded);
    }
    else if(disconnect_tracker_notify) {
        disconnect(Resources::Lib::TRACKER_MODEL, &TrackerTableModel::trackerAdded,
                this, &BaseTile::onTrackerAdded);
    }
}

void BaseTile::setActivateKey(const QChar &c)
{
    activate_key_ = c;
}

const QChar &BaseTile::getActivateKey() const
{
    return activate_key_;
}

void BaseTile::setSize(qreal size)
{
    prepareGeometryChange();
    size_ = size;
    /*QRectF r(boundingRect());
    if(r.width() > 5 && scene())
        scene()->update(scene()->sceneRect());*/
}

qreal BaseTile::getSize() const
{
    return size_;
}

void BaseTile::setSizeAnimated(qreal size, int duration)
{
    qreal prev_size = size_;

    setSizeLayoutAware(size);

    QPropertyAnimation* anim = new QPropertyAnimation(this, "size");
    anim->setStartValue(prev_size);
    anim->setEndValue(size);
    anim->setDuration(duration);
    anim->setEasingCurve(QEasingCurve::InOutQuad);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void BaseTile::setSizeLayoutAware(qreal size)
{
    qreal prev_size = size_;
    size_ = size;
    fixOverlapsAfterResize(prev_size);

    scene()->update(scene()->sceneRect());
}

void BaseTile::setPosAnimated(const QPointF& p, int duration)
{
    QPropertyAnimation* anim = new QPropertyAnimation(this, "pos");
    anim->setStartValue(pos());
    anim->setEndValue(p);
    anim->setDuration(duration);
    anim->setEasingCurve(QEasingCurve::InOutQuad);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void BaseTile::setName(const QString &str)
{
    name_ = str;
}

const QString &BaseTile::getName() const
{
    return name_;
}

const QMenu *BaseTile::getContextMenu() const
{
    return context_menu_;
}

const QUuid &BaseTile::getUuid() const
{
    return uuid_;
}

bool BaseTile::hasActivateKey() const
{
    return activate_key_ != ' ';
}

void BaseTile::receiveExternalData(const QMimeData *data)
{
    qDebug() << "Tile " << name_ <<" : Received Data "<< data->text();
}

void BaseTile::receiveWheelEvent(QWheelEvent*)
{}

const QJsonObject BaseTile::toJsonObject() const
{
    QJsonObject obj;

    obj["name"] = name_;
    obj["size"] = size_;
    QJsonArray arr_pos;
    arr_pos.append(pos().x());
    arr_pos.append(pos().y());
    obj["position"] = arr_pos;
    if(hasActivateKey())
        obj["activate_key"] = QString(activate_key_);
    if(getTrackableName().size() > 0)
        obj["trackable_name"] = getTrackableName();
    obj["uuid"] = uuid_.toString();

    return obj;
}

bool BaseTile::setFromJsonObject(const QJsonObject &obj)
{
    // check format
    if(obj.isEmpty())
        return false;
    if(!(obj.contains("name") && obj.contains("size") && obj.contains("position")))
        return false;
    if(!(obj["name"].isString() && obj["size"].isDouble() && obj["position"].isArray()))
        return false;

    QJsonArray arr_pos = obj["position"].toArray();
    if(obj["position"].toArray().size() != 2)
        return false;

    // set name, size and pos
    setName(obj["name"].toString());
    setSize((qreal) obj["size"].toDouble());
    setPos(QPointF(arr_pos[0].toDouble(), arr_pos[1].toDouble()));

    // set activate key
    if(obj.contains("activate_key") && obj["activate_key"].isString()) {
        QString k = obj["activate_key"].toString();
        if(k.size() == 1)
            setActivateKey(k.at(0));
    }

    // set uuid
    if(obj.contains("uuid") && obj["uuid"].isString())
        uuid_ = QUuid(obj["uuid"].toString());

    // set trackable name
    if(obj.contains("trackable_name") && obj["trackable_name"].isString())
        setTrackableName(obj["trackable_name"].toString());

    return true;
}

void BaseTile::loadOverlayPixmap(const QString &file_path)
{
    if(file_path.compare(overlay_pixmap_path_) == 0)
        return;

    QPixmap* p = new QPixmap(file_path);
    if(!p->isNull()) {
        clearOverlayPixmap();
        overlay_pixmap_ = p;
        overlay_pixmap_path_ = file_path;
    }
    else {
        QErrorMessage e;
        e.showMessage(tr("Could not create Image from given File. Please check given path:\n") + file_path);
        delete p;
    }
}

const QString &BaseTile::getOverlayPixmapPath() const
{
    return overlay_pixmap_path_;
}

void BaseTile::clearOverlayPixmap()
{
    if(overlay_pixmap_ != 0) {
        delete overlay_pixmap_;
        overlay_pixmap_ = 0;
        overlay_pixmap_path_.clear();
    }
}

bool BaseTile::isActivated() const
{
    return is_activated_;
}

void BaseTile::setActivated(bool state)
{
    if(state == is_activated_)
        return;
    onActivate();
}

void BaseTile::setPresetModel(DB::Model::PresetTableModel *model)
{
    preset_model_ = model;
}

DB::Model::PresetTableModel* BaseTile::getPresetModel()
{
    return preset_model_;
}

bool BaseTile::getIsSelected() const
{
    return is_selected_;
}

void BaseTile::setIsSelected(bool state)
{
    if(state == is_selected_)
        return;
    prepareGeometryChange();
    is_selected_ = state;
}

void BaseTile::toggleSelection()
{
    prepareGeometryChange();
    is_selected_ = !is_selected_;
}

void BaseTile::onActivate()
{
    is_activated_ = !is_activated_;
    emit activated();
    update();
}

void BaseTile::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
    if(e->button() == Qt::LeftButton) {
        if(e->modifiers() & Qt::ControlModifier) {
            toggleSelection();
            ctrl_clicked_ = true;
            long_click_timer_->start(long_click_duration_);
        }
        else {
            setMode(ACTIVATED);
            long_click_timer_->start(long_click_duration_);
        }
    }
    else if(e->button() == Qt::RightButton) {
        // qDebug() << "right button";
    }
    else if(e->button() == Qt::MidButton) {
        // qDebug() << "mid button";
    }

    QGraphicsItem::mousePressEvent(e);
    emit mousePressed(e);
}

void BaseTile::mouseReleaseEvent(QGraphicsSceneMouseEvent* e)
{
    setMode(IDLE);
    long_click_timer_->stop();
    ctrl_clicked_ = false;
    QGraphicsItem::mouseReleaseEvent(e);
    emit mouseReleased(e);
}

void BaseTile::mouseMoveEvent(QGraphicsSceneMouseEvent* e)
{
    if(mode_ == MOVE) {
        QPointF p = pos();
        QGraphicsItem::mouseMoveEvent(e);
        QPointF p_new = pos();

        QList<QGraphicsItem*> col_it = collidingItems(Qt::IntersectsItemBoundingRect);
        if(col_it.size() > 0) {
            qreal x_min = col_it[0]->pos().x() - boundingRect().width();
            qreal x_max = col_it[0]->pos().x() + col_it[0]->boundingRect().width();
            qreal y_min = col_it[0]->pos().y() - boundingRect().height();
            qreal y_max = col_it[0]->pos().y() + col_it[0]->boundingRect().height();

            QRectF rect(QPointF(x_min, y_min), QPointF(x_max, y_max));

            switch(closestSide(p_new, rect)) {
                case LEFT:
                    p_new.setX(x_min);
                    break;
                case RIGHT:
                    p_new.setX(x_max);
                    break;
                case UPPER:
                    p_new.setY(y_min);
                    break;
                case LOWER:
                    p_new.setY(y_max);
                    break;
            }
            setPos(p_new);
        }

        // check if item in scene bounds
        qreal max_x = scene()->width() - boundingRect().width();
        qreal max_y = scene()->height() - boundingRect().height();
        if (x() < 0)
            setPos(0, y());
        else if (x() > max_x)
            setPos(max_x, y());

        if (y() < 0)
            setPos(x(), 0);
        else if (y() > max_y)
            setPos(x(), max_y);

        // if still colliding set pos back to start
        col_it = collidingItems(Qt::IntersectsItemBoundingRect);
        if(col_it.size() > 0)
            setPos(p);
    }

    emit mouseMoved(e);
}

void BaseTile::hoverEnterEvent(QGraphicsSceneHoverEvent* e)
{
    if(mode_ == IDLE)
        setMode(HOVER);
    emit hoverEntered(e);
    e->accept();
}

void BaseTile::hoverLeaveEvent(QGraphicsSceneHoverEvent* e)
{
    if(mode_ == HOVER)
        setMode(IDLE);
    emit hoverLeft(e);
    e->accept();
}

void BaseTile::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    //QGraphicsItem::dragEnterEvent(event);
    event->setDropAction(Qt::CopyAction);
    event->accept();
}

void BaseTile::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    QGraphicsItem::dragLeaveEvent(event);
}

void BaseTile::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    QGraphicsItem::dragMoveEvent(event);
    event->setDropAction(Qt::CopyAction);
    event->accept();
}

void BaseTile::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    QGraphicsItem::dropEvent(event);
}

void BaseTile::performDrag()
{
    if(!scene())
        return;

    QJsonDocument doc;
    QJsonObject obj;
    QJsonObject obj_data = toJsonObject();
    obj["data"] = obj_data;
    obj["type"] = metaObject()->className();
    doc.setObject(obj);

    QMimeData* mime_data = new QMimeData;
    mime_data->setText(QString(doc.toJson()));

    // create Drag
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mime_data);
    drag->setPixmap(*Resources::Lib::PX_SOUND_FILE_DRAG);

    scene()->removeItem(this);

    // will block until drag done
    drag->exec(Qt::CopyAction);

    deleteLater();
}

void BaseTile::trackableSourceAddedEvent(Tracker *t)
{
    auto act_t = dynamic_cast<ActivationTracker*>(t);
    if(!act_t)
        return;
    act_t->link(this);
}

void BaseTile::contextMenuEvent(QGraphicsSceneContextMenuEvent *e)
{
    context_menu_->popup(e->screenPos());
}

void BaseTile::fixOverlapsAfterResize(qreal prev_size)
{
    if(prev_size >= size_)
        return;

    QList<QGraphicsItem*> cols = collidingItems(Qt::IntersectsItemShape);
    // no collision, keep layout
    if(cols.size() == 0)
        return;

    // all movement animations will have to be perfromed
    // after seting the psoitions statically for quick intersect computation
    QMap<QGraphicsItem*,QPropertyAnimation*> animations;

    // fix overlapping
    qreal dist = abs(size_ * 100 - prev_size * 100);
    bool collision = true;
    QSet<QGraphicsItem*> items = scene()->items().toSet();
    items.remove(this);
    while(collision) {
        collision = false;
        QSet<QGraphicsItem*> remove_items;
        foreach(QGraphicsItem* it, items) {
            // cast necessary for property animation
            BaseTile* c_it = dynamic_cast<BaseTile*>(it);

            // skip non GraphicsItems
            if(!c_it)
                continue;

            /*if(c_it->x() < x()-c_it->boundingRect().width() || c_it->y() < y()-c_it->boundingRect().height()) {
                remove_items.insert(it);
                continue;
            }*/

            // do not move non colliding
            if(c_it->collidingItems(Qt::IntersectsItemShape).size() == 0)
                continue;

            // move item along extension of sized item
            QPointF start = c_it->pos();
            QPointF end(c_it->pos().x()+dist, c_it->pos().y()+dist);

            // start and end might be same for started animations
            //if((start - end).manhattanLength() < 0.0001)
            //    continue;

            // actual new collsion found
            collision = true;

            if(!animations.contains(it)) {
                // store positional change animation
                QPropertyAnimation* anim = new QPropertyAnimation(c_it, "pos");
                anim->setDuration(300);
                anim->setStartValue(start);
                anim->setEndValue(end);
                anim->setEasingCurve(QEasingCurve::InOutQuad);
                animations[it] = anim;
            }

            // set final pos statically for now
            // overlap computation will be easier that way
            // after all final positions are reached
            // position change will be animated again
            c_it->setPos(end);

            // add to list of moved items
            // (use 'it' since 'c_it' is casted)
            remove_items.insert(it);

            // extend scene bounds if item gets pushed out of screen
            // x
            qreal max_x = scene()->width() - c_it->boundingRect().width();
            if (end.x() > max_x) {
                qreal screen_ext = max_x - end.x();
                QRectF rect = scene()->sceneRect();
                rect.setWidth(rect.width() + abs(screen_ext));
                scene()->setSceneRect(rect);
            }
            // y
            qreal max_y = scene()->height() - c_it->boundingRect().height();
            if (end.y() > max_y) {
                qreal screen_ext = max_y - end.y();
                QRectF rect = scene()->sceneRect();
                rect.setHeight(rect.height() + abs(screen_ext));
                scene()->setSceneRect(rect);
            }
        }

        // limit items to the ones, which have not been moved
        items.subtract(remove_items);
    }

    // trigger transitions
    foreach(QPropertyAnimation* anim, animations.values())
        anim->start(QAbstractAnimation::DeleteWhenStopped);

}

const QRectF BaseTile::getPaintRect() const
{
    QRectF paint_rect(
        boundingRect().x()+OFFSET,
        boundingRect().y()+OFFSET,
        qMax(boundingRect().width()-2.0f*OFFSET, 0.0),
        qMax(boundingRect().width()-2.0f*OFFSET, 0.0)
    );
    return paint_rect;
}

const QRectF BaseTile::getTextRect() const
{
    QRectF text_rect(
        getPaintRect().x(),
        getPaintRect().y()+getPaintRect().height(),
        getPaintRect().width(),
        TEXT_HEIGHT
    );
    return text_rect;
}

const QBrush BaseTile::getBackgroundBrush() const
{
    QBrush b(Qt::gray);

    switch(mode_) {
        case ACTIVATED:
            b.setColor(Qt::green);
            break;

        default:
            break;
    }

    return b;
}

const QPixmap BaseTile::getOverlayPixmap() const
{
    if (overlay_pixmap_ != 0)
        return *overlay_pixmap_;

    if(mode_ == ACTIVATED)
        return *Resources::Lib::PX_CRACKED_STONE_INV;
    else
        return *Resources::Lib::PX_CRACKED_STONE;
}

const QPixmap BaseTile::getActivatePixmap() const
{
    QPixmap* px = Resources::Lib::getKeyPixmap(activate_key_);
    if(px == 0)
        return QPixmap();
    return *px;
}

void BaseTile::setDefaultOpacity()
{
    switch(mode_) {
        case MOVE:
            setOpacity(0.2);
            break;

        default:
            setOpacity(1.0);
            break;
    }

}

void BaseTile::setMode(BaseTile::ItemMode mode)
{
    prepareGeometryChange();
    mode_ = mode;
}

void BaseTile::setSmallSize()
{
    setSizeAnimated(1);
}

void BaseTile::setMediumSize()
{
    setSizeAnimated(2);
}

void BaseTile::setLargeSize()
{
    setSizeAnimated(3);
}

void BaseTile::onLongClick()
{
    if(ctrl_clicked_)
        performDrag();
    else
        setMode(MOVE);
    long_click_timer_->stop();
}

void BaseTile::onDelete()
{
    scene()->removeItem(this);
    deleteLater();
}

void BaseTile::onSaveAsPreset()
{
    if(preset_model_ == 0)
        return;

    DB::PresetRecord* rec = preset_model_->getPresetByName(name_);
    if(rec != 0) {
        QMessageBox msg_box;
        msg_box.setIcon(QMessageBox::Warning);
        msg_box.setText("A Preset named '" + name_ + "' already exists.");
        msg_box.setInformativeText("Please rename your Tile or delete the existing Preset first.");
        msg_box.exec();
        return;
    }

    QJsonDocument doc;
    QJsonObject obj;
    QJsonObject obj_data = toJsonObject();
    if(obj_data.contains("uuid"))
        obj_data.remove("uuid");
    obj["data"] = obj_data;
    obj["type"] = metaObject()->className();
    doc.setObject(obj);
    preset_model_->addPresetRecord(name_, QString(doc.toJson()));
}

void BaseTile::onSetKey()
{
    Misc::CharInputDialog d;
    d.setChar(getActivateKey());
    if(d.exec()) {
        setActivateKey(d.getChar());
        activate_action_->setShortcut(QKeySequence(QString(d.getChar())));
    }
}

void BaseTile::onSetActivationTracker()
{
    TrackerPickerDialog d(ActivationTracker::ACTIVE_STATE);
    Tracker* current_tracker = 0;
    if(hasLink(ActivationTracker::ACTIVE_STATE)) {
        current_tracker = links_[ActivationTracker::ACTIVE_STATE];
        d.setTracker(current_tracker);
        setTrackableName(current_tracker->getName());
    }

    int ret = d.exec();
    if(ret != QDialog::Accepted)
        return;

    Tracker* t = d.getTracker();
    if(!t || t == current_tracker)
        return;

    auto act_t = dynamic_cast<ActivationTracker*>(t);
    if(!act_t)
        return;

    if(current_tracker) {
        QMessageBox b;
        b.setText(tr("You are about to override the current activation tracker '")+current_tracker->getName()+tr("'."));
        b.setInformativeText(tr("Do you wish to proceed?"));
        b.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        b.setDefaultButton(QMessageBox::No);
        if(b.exec() != QMessageBox::Yes)
            return;
    }

    setTrackableName(act_t->getName());
    act_t->link(this);
}

void BaseTile::onTrackerAdded(const QString &name)
{
    if(name.compare(trackable_name_) != 0)
        return;

    Tracker* current_tracker = 0;
    if(hasLink(ActivationTracker::ACTIVE_STATE))
        current_tracker = links_[ActivationTracker::ACTIVE_STATE];

    Tracker* t = Resources::Lib::TRACKER_MODEL->getTracker(getTrackableName());
    if(!t || t == current_tracker)
        return;

    trackableSourceAddedEvent(t);
}

qreal BaseTile::distance(const QPointF &p, const QLineF &l)
{
    // transform to loocal coordinates system (0,0) - (lx, ly)
    QPointF p1 = l.p1();
    QPointF p2 = l.p2();
    qreal x = p.x() - p1.x();
    qreal y = p.y() - p1.y();
    qreal x2 = p2.x() - p1.x();
    qreal y2 = p2.y() - p1.y();

    // if line is a point (nodes are the same) =>
    // just return distance between point and one line node
    qreal norm = sqrt(x2*x2 + y2*y2);
    if (norm <= std::numeric_limits<int>::epsilon())
      return sqrt(x*x + y*y);

    // distance
    return fabs(x*y2 - y*x2) / norm;
}

BaseTile::BOX_SIDE BaseTile::closestSide(const QPointF &p, const QRectF &rect)
{
    qreal x_min = rect.x();
    qreal x_max = rect.x() + rect.width();
    qreal y_min = rect.y();
    qreal y_max = rect.y() + rect.height();
    qreal temp_dist = 0;

    // left
    QLineF l(QPointF(x_min, y_min), QPointF(x_min, y_max));
    qreal min_dist = distance(p,l);
    BOX_SIDE side = LEFT;

    // right
    l.setPoints(QPointF(x_max,y_min), QPointF(x_max, y_max));
    temp_dist = distance(p,l);
    if(temp_dist < min_dist) {
        min_dist = temp_dist;
        side = RIGHT;
    }

    // upper
    l.setPoints(QPointF(x_min, y_min), QPointF(x_max, y_min));
    temp_dist = distance(p,l);
    if(temp_dist < min_dist) {
        min_dist = temp_dist;
        side = UPPER;
    }

    // lower
    l.setPoints(QPointF(x_min, y_max), QPointF(x_max, y_max));
    temp_dist = distance(p,l);
    if(temp_dist < min_dist) {
        min_dist = temp_dist;
        side = LOWER;
    }

    return side;
}

void BaseTile::createContextMenu()
{
    // create size actions
    QAction* small_size_action = new QAction(tr("Small"), this);
    QAction* medium_size_action = new QAction(tr("Medium"), this);
    QAction* large_size_action = new QAction(tr("Large"), this);

    connect(small_size_action, SIGNAL(triggered()),
            this, SLOT(setSmallSize()));
    connect(medium_size_action, SIGNAL(triggered()),
            this, SLOT(setMediumSize()));
    connect(large_size_action, SIGNAL(triggered()),
            this, SLOT(setLargeSize()));

    // create size menu
    QMenu* size_menu = new QMenu(tr("Size"));
    size_menu->addAction(small_size_action);
    size_menu->addAction(medium_size_action);
    size_menu->addAction(large_size_action);

    // create delete action
    QAction* delete_action = new QAction(tr("Delete"), this);
    connect(delete_action, SIGNAL(triggered()),
            this, SLOT(onDelete()));

    // change activate button
    QAction* activate_button_action = new QAction(tr("Set Key..."), this);
    connect(activate_button_action, SIGNAL(triggered()),
            this, SLOT(onSetKey()));

    // create delete action
    QAction* save_as_preset_action = new QAction(tr("Save As Preset"), this);
    connect(save_as_preset_action, SIGNAL(triggered()),
            this, SLOT(onSaveAsPreset()));

    // create activation trigger
    QAction* activate_tracker_action = new QAction(tr("Set Activate Tracker..."), this);
    connect(activate_tracker_action, &QAction::triggered,
            this, &BaseTile::onSetActivationTracker);

    // create context menu
    //context_menu_->addAction(activate_action_);
    context_menu_->addAction(activate_button_action);
    context_menu_->addAction(activate_tracker_action);
    context_menu_->addMenu(size_menu);
    context_menu_->addSeparator();
    context_menu_->addAction(save_as_preset_action);
    context_menu_->addSeparator();
    context_menu_->addAction(delete_action);
}

} // namespace Tile
