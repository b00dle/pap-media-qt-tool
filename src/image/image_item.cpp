#include "image_item.h"

#include <QPixmap>

ImageItem::ImageItem(const QString& path, QGraphicsItem *parent)
    : QGraphicsPixmapItem(QPixmap(path), parent)
    , path_(path)
{}

const QString &ImageItem::getPath() const
{
    return path_;
}
