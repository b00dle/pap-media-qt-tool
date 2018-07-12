#include "interactive_image_shape.h"

#include <QPainter>

InteractiveImageShape::InteractiveImageShape(const QPainterPath& path, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , path_(path)
{}

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
    painter->fillPath(path_, QColor(QColor(55,55,56)));
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