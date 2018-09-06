#ifndef INTERACTIVE_IMAGE_TOKEN_H
#define INTERACTIVE_IMAGE_TOKEN_H

#include <QGraphicsObject>
#include <QUuid>
#include <QJsonObject>

#include "tracking/trackable.h"

class InteractiveImageToken : public QGraphicsObject, public Trackable
{
    Q_OBJECT

    Q_INTERFACES(QGraphicsItem) // so instances can be casted using qobject_cast

public:
    enum State {
        IDLE,
        SELECTED,
        MOVE
    };

    enum { Type = UserType + 79 };
    virtual int type() const { return Type; }

public:
    explicit InteractiveImageToken(QGraphicsItem *parent = nullptr);
    explicit InteractiveImageToken(const QSizeF& s, QGraphicsItem *parent = nullptr);
    virtual ~InteractiveImageToken();

    /**
     * See BC.
    */
    virtual QRectF boundingRect() const;

    /***/
    virtual QRectF markerRect() const;

    /***/
    virtual QRectF textRect() const;

    /***/
    virtual QRectF grabRect() const;

    /**
     * See BC.
    */
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    /**
     * See BC.
    */
    virtual bool updateLinkFromTracker(Tracker *tracker, int target_prop);

    /**
     * See BC.
    */
    virtual bool updateGrabFromTracker(Tracker *tracker, int target_prop);

    /**
     * See BC.
    */
    virtual bool registerGrab(Tracker* tracker, int target_prop);

    /**
     * See BC.
     */
    virtual bool registerLink(Tracker *tracker, int target_prop);

    /**
     * Returns a QJsonObject holding all information about the token
    */
    virtual const QJsonObject toJsonObject() const;

    /**
     * Set all values held by JSON object.
     * Returns success of parsing JsonObject.
    */
    virtual bool setFromJsonObject(const QJsonObject& obj);

    /*
    * Returns a boundingRect for the current uncover area.
    * Definition is in item coordinates.
    */
    virtual const QRectF uncoverBoundingRect() const;

    /**
     * Gets the name for this instance.
    */
    const QString& getName() const;

    /**
     * Sets the name for this instance.
    */
    void setName(const QString& n);

    /*
    * Returns the uuid of this instance.
    */
    const QUuid& getUuid() const;

    /**
     * Calculates the overall bounding rect
     */
    const QRectF calculateBoundingRect() const;

    /*
    * Returns the radius from center of bounds at which
    * this item should uncover an image item.
    * Default is sqrt(br.width()*br.width + br.height()*br.height()).
    * Can be set from setUncoverRadius();
    */
    float getUncoverRadius() const;

    /*
    * Sets the radius from center of bounds at which
    * this item should uncover an image item.
    */
    void setUncoverRadius(float r);

    /**
    * Returns the radius from center of bounds at which
    * this item should be grabbable
     */
    float getGrabRadius() const;

    /**
     * Sets the distance from center at which
     * the token can be grabbed.
     */
    void setGrabRadius(float d);

    /**
     * Returns, whether the grab ellipse is shown.
     */
    bool getShowGrabIndicator() const;

    /**
     * Sets, whether the grab ellipse is shown.
     */
    void setShowGrabIndicator(bool);

    /**
     * Returns, whether the uncover ellipse is shown
     */
    bool getShowUncoverIndicator() const;

    /**
     * Sets, whether the uncover ellipse is shown.
     */
    void setShowUncoverIndicator(bool show);

    /**
     * Returns the tokens currently set color.
     */
    const QColor &getColor() const;

    /**
     * Sets the tokens color.
     */
    void setColor(const QColor &clr);

    /*
    * Sets a new bounding rect of this item from [0,0] to [s.width, s.height].
    */
    void setSize(const QSizeF& s);

    /*
    * Returns the bounding rect center pos in scene coordinates.
    */
    const QPointF centerPos() const;

    /**
     * Sets the position of this item and emits the hasMoved signal
    */
    void setUncoverPos(const QPointF& pos);

signals:
    void hasMoved();
    void uncoverRadiusChanged();

public slots:

protected:
    bool ensureGrabbable(Tracker* tracker, int target_prop);

    void setState(State);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent* e);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* e);

    QRectF marker_rect_;
    QRectF grab_rect_;
    QRectF uncover_rect_;

    State state_;
    QUuid uuid_;
    float uncover_radius_;
    QString name_;
    QColor color_;

    bool show_grab_indicator_;
    bool show_uncover_indicator_;

    float grab_radius_;
    float grabbed_rotation_;
    bool token_grabbed_;
    QPointF grabbed_position_;
    QPointF grabbed_relative_position_;

};

#endif // INTERACTIVE_IMAGE_TOKEN_H
