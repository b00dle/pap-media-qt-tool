#ifndef TUIO_TOKEN_ITEM_H
#define TUIO_TOKEN_ITEM_H

#include <QGraphicsItem>

class TuioTokenItem : public QGraphicsItem
{
public:
    TuioTokenItem(QGraphicsItem* parent = nullptr);
    TuioTokenItem(int id, int class_id, QGraphicsItem* parent = nullptr);
    virtual ~TuioTokenItem();

    /**
     * See BC.
    */
    virtual QRectF boundingRect() const;

    /**
     * See BC.
    */
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    int getID() const;
    int getClassID() const;

    void setID(int id);
    void setClassID(int id);

private:
    int id_;
    int class_id_;
};

#endif // TUIO_TOKEN_ITEM_H
