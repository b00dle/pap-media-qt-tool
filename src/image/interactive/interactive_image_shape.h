#ifndef INTERACTIVE_IMAGE_SHAPE_H
#define INTERACTIVE_IMAGE_SHAPE_H

#include <QGraphicsPathItem>
#include <QJsonObject>
#include <QObject>

#include "tracking/activation_tracker.h"

class InteractiveImageShape : public QGraphicsObject, public ActivationTracker
{
    Q_OBJECT

    Q_INTERFACES(QGraphicsItem) // so instances can be casted using qobject_cast

public:
    enum { Type = UserType + 80 };
    virtual int type() const { return Type; }

public:
    InteractiveImageShape(const QPainterPath& path, QGraphicsItem* parent = 0);
    InteractiveImageShape(QGraphicsItem* parent = 0);
    virtual ~InteractiveImageShape();

    virtual void setName(const QString&);

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget);
    virtual QPainterPath shape() const;

    void setPath(const QPainterPath& p);

    void setUncoverEnabled(bool enabled);
    bool getUncoverEnabled() const;

    void setFogVisibility(bool enabled);
    bool isVisibleInFog() const;

    /**
     * Returns a QJsonObject holding all information about the token
    */
    virtual const QJsonObject toJsonObject() const;

    /**
     * Set all values held by JSON object.
     * Returns success of parsing JsonObject.
    */
    virtual bool setFromJsonObject(const QJsonObject& obj);

signals:
    void fogVisibilityChanged();

protected:
    QPainterPath path_;
    bool is_uncover_shape_;
    bool is_visible_in_fog_;
};

#endif // INTERACTIVE_IMAGE_SHAPE_H
