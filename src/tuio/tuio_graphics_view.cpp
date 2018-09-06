#include "tuio_graphics_view.h"

#include <QResizeEvent>
#include <QGraphicsItem>
#include <QDebug>

TuioGraphicsView::TuioGraphicsView(QWidget *parent)
    : QGraphicsView(parent)
    , default_size_(800, 600)
{
    setScene(new QGraphicsScene(0,0,default_size_.width(),default_size_.height(), this));
}

void TuioGraphicsView::addItem(QGraphicsItem *it)
{
    float scale_factor = qMin(
        scene()->width() / (float) default_size_.width(),
        scene()->height() / (float) default_size_.height()
    );
    it->setScale(scale_factor);
    scene()->addItem(it);
}

void TuioGraphicsView::resizeEvent(QResizeEvent *e)
{
    QGraphicsView::resizeEvent(e);
    if(!e->isAccepted())
        return;
    QRectF r = scene()->sceneRect();
    QMap<QGraphicsItem*, QPointF> new_pos;
    float scale_factor = qMin(
        e->size().width() / (float) default_size_.width(),
        e->size().height() / (float) default_size_.height()
    );
    foreach(auto it, scene()->items()) {
        new_pos[it] = QPointF(
            (it->pos().x() / r.width()) * e->size().width(),
            (it->pos().y() / r.height()) * e->size().height()
        );
    }
    r.setWidth(e->size().width());
    r.setHeight(e->size().height());
    scene()->setSceneRect(r);
    foreach(auto it, new_pos.keys()) {
        it->setPos(new_pos[it]);
        it->setScale(scale_factor);
    }
}
