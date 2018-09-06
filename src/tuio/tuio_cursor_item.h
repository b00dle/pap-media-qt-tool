#ifndef TUIO_CURSOR_ITEM_H
#define TUIO_CURSOR_ITEM_H

#include <QGraphicsItem>

class TuioCursorItem : public QGraphicsItem
{
public:
    TuioCursorItem(QGraphicsItem* parent = nullptr);
    TuioCursorItem(int id, QGraphicsItem* parent = nullptr);
    virtual ~TuioCursorItem();

    /**
     * See BC.
    */
    virtual QRectF boundingRect() const;

    /**
     * See BC.
    */
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    int getID() const;
    void setID(int id);

private:
    int id_;
};

#endif // TUIO_CURSOR_ITEM_H
