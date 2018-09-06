#ifndef IMAGE_ITEM_H
#define IMAGE_ITEM_H

#include <QGraphicsPixmapItem>

class ImageItem : public QGraphicsPixmapItem
{
public:
    enum { Type = UserType + 77 };
    virtual int type() const { return Type; }

public:
    explicit ImageItem(const QString& path, QGraphicsItem *parent = nullptr);

    const QString& getPath() const;

private:
    QString path_;
};

#endif // IMAGE_ITEM_H
