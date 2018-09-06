#include "tuio_blob_item.h"

#include <QPainter>

TuioBlobItem::TuioBlobItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , bounding_rect_()
    , id_(-1)
{}

TuioBlobItem::TuioBlobItem(int id, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , id_(id)
{}

TuioBlobItem::~TuioBlobItem()
{}

QRectF TuioBlobItem::boundingRect() const
{
    return bounding_rect_;
}

void TuioBlobItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QRectF b_rect(boundingRect());
    painter->setPen(QColor(55,55,56));
    painter->setBrush(QColor(55,55,56));
    painter->drawEllipse(bounding_rect_.center(), bounding_rect_.width()/2.0, bounding_rect_.height()/2.0);
    painter->setPen(QColor(Qt::white));
    painter->drawText(b_rect, Qt::TextWrapAnywhere | Qt::AlignCenter, "id " + QString::number(id_));
}

void TuioBlobItem::setBoundingRect(const QSize &size)
{
    bounding_rect_ = QRectF(-size.width()/2, -size.height()/2, size.width(), size.height());
}

int TuioBlobItem::getID() const
{
    return id_;
}

void TuioBlobItem::setID(int id)
{
    prepareGeometryChange();
    id_ = id;
}
