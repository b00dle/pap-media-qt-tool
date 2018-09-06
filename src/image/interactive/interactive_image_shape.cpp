#include "interactive_image_shape.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QDebug>
#include <QJsonArray>

#include "resources/lib.h"
#include "misc/json_mime_data_parser.h"

InteractiveImageShape::InteractiveImageShape(const QPainterPath& path, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , ActivationTracker()
    , path_(path)
    , is_uncover_shape_(true)
    , is_visible_in_fog_(true)
{
    setCleanByModelEnabled(false);
}

InteractiveImageShape::InteractiveImageShape(QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , ActivationTracker()
    , path_()
    , is_uncover_shape_(true)
    , is_visible_in_fog_(true)
{
    setCleanByModelEnabled(false);
}

InteractiveImageShape::~InteractiveImageShape()
{
    if(scene())
        scene()->removeItem(this);
    if(Resources::Lib::TRACKER_MODEL->hasTracker(this))
        Resources::Lib::TRACKER_MODEL->removeTracker(this);
}

void InteractiveImageShape::setName(const QString &n)
{
    prepareGeometryChange();
    ActivationTracker::setName(n);
}

QRectF InteractiveImageShape::boundingRect() const
{
    return path_.boundingRect();
}

void InteractiveImageShape::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPen p(painter->pen());
    p.setWidth(5);
    p.setColor(QColor(155,155,156));
    painter->setPen(p);
    painter->drawPath(path_);
    if(isSelected())
        painter->fillPath(path_, QColor(QColor(55,155,56)));
    else
        painter->fillPath(path_, QColor(QColor(55,55,56)));
    if(getName().size() > 0) {
        QFont f = painter->font();
        f.setPixelSize(30);
        painter->setFont(f);
        p.setColor(QColor(Qt::white));
        painter->drawText(boundingRect(), getName(), QTextOption(Qt::AlignLeft | Qt::AlignTop));
    }
}

QPainterPath InteractiveImageShape::shape() const
{
    return path_.simplified();
}

void InteractiveImageShape::setPath(const QPainterPath &p)
{
    prepareGeometryChange();
    path_ = p;
}

void InteractiveImageShape::setUncoverEnabled(bool enabled)
{
    is_uncover_shape_ = enabled;
}

bool InteractiveImageShape::getUncoverEnabled() const
{
    return is_uncover_shape_;
}

void InteractiveImageShape::setFogVisibility(bool enabled)
{
    if(enabled == is_visible_in_fog_)
        return;

    is_visible_in_fog_ = enabled;
    emit fogVisibilityChanged();
}

bool InteractiveImageShape::isVisibleInFog() const
{
    return is_visible_in_fog_;
}

const QJsonObject InteractiveImageShape::toJsonObject() const
{
    QJsonObject obj;
    obj["name"] = name_;
    obj["path"] = Misc::JsonMimeDataParser::toJsonArray(path_);

    QJsonArray pos_arr;
    pos_arr.append(pos().x());
    pos_arr.append(pos().y());
    obj["position"] = pos_arr;

    obj["is_uncover_shape"] = is_uncover_shape_;
    obj["is_visible_in_fog"] = is_visible_in_fog_;
    obj["is_tracker"] = Resources::Lib::TRACKER_MODEL->hasTracker(name_);

    return obj;
}

bool InteractiveImageShape::setFromJsonObject(const QJsonObject &obj)
{
    if(obj.isEmpty())
        return false;

    bool well_formed = obj.contains("name") && obj["name"].isString() &&
        obj.contains("is_uncover_shape") && obj["is_uncover_shape"].isBool() &&
        obj.contains("is_visible_in_fog") && obj["is_visible_in_fog"].isBool() &&
        obj.contains("is_tracker") && obj["is_tracker"].isBool() &&
        obj.contains("position") && obj["position"].isArray() &&
        obj.contains("path") && obj["path"].isArray();

    if(!well_formed)
        return false;

    QJsonArray arr_pos = obj["position"].toArray();
    if(obj["position"].toArray().size() != 2)
        return false;

    setPath(Misc::JsonMimeDataParser::toPainterPath(obj["path"].toArray()));
    setUncoverEnabled(obj["is_uncover_shape"].toBool());
    setFogVisibility(obj["is_visible_in_fog"].toBool());
    setName(obj["name"].toString());
    setPos(QPointF(arr_pos[0].toDouble(),arr_pos[1].toDouble()));

    if(obj["is_tracker"].toBool())
        Resources::Lib::TRACKER_MODEL->addTracker(this);

    return true;
}
