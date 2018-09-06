#ifndef TUIO_BLOB_ITEM_H
#define TUIO_BLOB_ITEM_H

#include <QGraphicsItem>

class TuioBlobItem: public QGraphicsItem
{
    public:
        TuioBlobItem(QGraphicsItem* parent = nullptr);
        TuioBlobItem(int id, QGraphicsItem* parent = nullptr);
        virtual ~TuioBlobItem();

        /**
         * See BC.
         */
        virtual QRectF boundingRect() const;

        /**
         * See BC.
         */
        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

        void setBoundingRect(const QSize& size);

    int getID() const;
    void setID(int id);

private:

    QRectF bounding_rect_;
    int id_;

};

#endif // TUIO_BLOB_ITEM_H
