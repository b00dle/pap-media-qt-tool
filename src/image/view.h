#ifndef IMAGE_VIEW_H
#define IMAGE_VIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QShowEvent>
#include <QMenu>

class ImageItem;
class InteractiveImage;

namespace Image {

class View : public QGraphicsView
{
    Q_OBJECT
public:
    explicit View(QWidget *parent = 0);
    virtual  ~View();

    void setItem(ImageItem*);
    void setItem(InteractiveImage*);
    QGraphicsItem *getItem() const;

    bool isImageInteractive() const;

    QMenu* getMenuBarExtension();

signals:
    void itemSet();
    void interactiveEnabled(bool enabled = true);

public slots:
    void clear();
    void scaleContentsToViewport();
    void onMakeInteractive();

private slots:
    void onNewContentsLoaded();

private:
    void setItem(QGraphicsItem*);

    void resizeEvent(QResizeEvent *event);
    const QRectF getVisibleRect() const;
    void mousePressEvent(QMouseEvent *event);
    /**
     * @brief initializes the context menu of this view.
     */
    void initContextMenu();

    QGraphicsItem* item_;
    QMenu* context_menu_;
};

} // namespace Image

#endif // IMAGE_VIEW_H
