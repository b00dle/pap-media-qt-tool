#include "view.h"

#include <QGraphicsScene>
#include <QBoxLayout>
#include <QKeyEvent>
#include <QDebug>
#include <QScrollBar>
#include <QCoreApplication>

#include "image_item.h"
#include "interactive/interactive_image.h"

namespace Image {

View::View(QWidget *parent)
    : QGraphicsView(parent)
    , item_(0)
    , context_menu_(0)
{
    setScene(new QGraphicsScene(this));
    setStyleSheet("background-color: #373738; color: white;");
    initContextMenu();
}

View::~View()
{
    context_menu_->deleteLater();
}

void View::setItem(ImageItem* it)
{
    setItem((QGraphicsItem*) it);
}

void View::setItem(InteractiveImage *it)
{
    setItem((QGraphicsItem*) it);
}

QGraphicsItem *View::getItem() const
{
    return item_;
}

bool View::isImageInteractive() const
{
    return qgraphicsitem_cast<InteractiveImage*>(item_) ? true : false;
}

QMenu *View::getMenuBarExtension()
{
    auto it = qgraphicsitem_cast<InteractiveImage*>(item_);
    if(it) {
        return it->getMenuBarExtension();
    }
    else {
        return context_menu_;
    }
}

void View::setItem(QGraphicsItem* item)
{
    clear();
    scene()->addItem(item);
    scene()->setSceneRect(item->boundingRect());
    scaleContentsToViewport();
    item_ = item;
    if(isImageInteractive()) {
        emit interactiveEnabled(true);
        auto iit = qgraphicsitem_cast<InteractiveImage*>(item_);
        connect(iit, &InteractiveImage::newContentsLoaded,
                this, &View::onNewContentsLoaded);
    }
    emit itemSet();
}

void View::clear()
{
    if(isImageInteractive()) {
        auto iit = qgraphicsitem_cast<InteractiveImage*>(item_);
        emit iit->destroyed();
        QCoreApplication::processEvents();
    }
    item_ = 0;
    scene()->clear();
    //scene()->setSceneRect(QRectF(0,0,0,0));
}

void View::scaleContentsToViewport()
{
    QRectF visible_rect = getVisibleRect();
    qreal scale_factor = qMin(
        visible_rect.width() / scene()->sceneRect().width(),
        visible_rect.height() / scene()->sceneRect().height()
    );
    scale(scale_factor, scale_factor);
}

void View::onMakeInteractive()
{
    auto it = qgraphicsitem_cast<ImageItem*>(item_);
    if(it) {
        QString path = it->getPath();
        auto interactive_img = new InteractiveImage(path, it->boundingRect().size().toSize());
        setItem(interactive_img);
        connect(interactive_img, &InteractiveImage::newContentsLoaded,
                this, &View::onNewContentsLoaded);
    }
}

void View::onNewContentsLoaded()
{
    scene()->setSceneRect(item_->boundingRect());
    scaleContentsToViewport();
}

void View::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if(item_)
        scene()->setSceneRect(item_->boundingRect());
    scaleContentsToViewport();
}

const QRectF View::getVisibleRect() const
{
    QPointF tl(horizontalScrollBar()->value(), verticalScrollBar()->value());
    QPointF br = tl + viewport()->rect().bottomRight();
    QMatrix mat = matrix().inverted();
    return mat.mapRect(QRectF(tl,br));
}

void View::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    if(!event->isAccepted()) {
        if(event->button() == Qt::RightButton) {
            context_menu_->popup(event->globalPos());
            event->accept();
        }
    }
}

void View::initContextMenu()
{
    context_menu_ = new QMenu(tr("Actions"));

    QAction* cover_image = new QAction(tr("Make Interactive"));
    connect(cover_image, SIGNAL(triggered()),
            this, SLOT(onMakeInteractive()));

    context_menu_->addAction(cover_image);
}

} // namespace Image
