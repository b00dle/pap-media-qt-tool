#include "tuio_token_item.h"

#include <QPainter>

TuioTokenItem::TuioTokenItem(QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , id_(-1)
    , class_id_(-1)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
}

TuioTokenItem::TuioTokenItem(int id, int class_id, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , id_(id)
    , class_id_(class_id)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
}

TuioTokenItem::~TuioTokenItem()
{}

QRectF TuioTokenItem::boundingRect() const
{
    return QRectF(-25,-25,50,50);
}

void TuioTokenItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QRectF b_rect(boundingRect());
    QRectF id_rect(b_rect);
    id_rect.setBottomRight(
        QPointF(
            id_rect.bottomRight().x(),
            id_rect.bottomRight().y() - b_rect.height()/2.0
        )
    );
    QRectF class_id_rect(b_rect);
    class_id_rect.setTopLeft(id_rect.bottomLeft());

    if(isSelected())
        painter->fillRect(b_rect, QBrush(QColor(50,152,253)));
    else
        painter->fillRect(b_rect, QBrush(QColor(55,55,56)));
    painter->setPen(QColor(Qt::white));
    painter->drawText(id_rect, Qt::TextWrapAnywhere | Qt::AlignCenter, "id " + QString::number(id_));
    painter->drawText(class_id_rect, Qt::TextWrapAnywhere | Qt::AlignCenter, "class " + QString::number(class_id_));
}

int TuioTokenItem::getID() const
{
    return id_;
}

int TuioTokenItem::getClassID() const
{
    return class_id_;
}

void TuioTokenItem::setID(int id)
{
    prepareGeometryChange();
    id_ = id;
}

void TuioTokenItem::setClassID(int id)
{
    prepareGeometryChange();
    class_id_ = id;
}
