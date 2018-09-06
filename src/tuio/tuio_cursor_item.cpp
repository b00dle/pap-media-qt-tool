#include "tuio_cursor_item.h"

#include <QPainter>

TuioCursorItem::TuioCursorItem(QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , id_(-1)
{}

TuioCursorItem::TuioCursorItem(int id, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , id_(id)
{}

TuioCursorItem::~TuioCursorItem()
{}

QRectF TuioCursorItem::boundingRect() const
{
    return QRectF(-17.5,-17.5,35,35);
}

void TuioCursorItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QRectF b_rect(boundingRect());
    painter->setPen(QColor(55,55,56));
    painter->setBrush(QColor(55,55,56));
    painter->drawEllipse(b_rect.center(), b_rect.width()/2.0, b_rect.height()/2.0);
    painter->setPen(QColor(Qt::white));
    painter->drawText(b_rect, Qt::TextWrapAnywhere | Qt::AlignCenter, "id " + QString::number(id_));
}

int TuioCursorItem::getID() const
{
    return id_;
}

void TuioCursorItem::setID(int id)
{
    prepareGeometryChange();
    id_ = id;
}
