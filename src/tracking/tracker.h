#ifndef TRACKER_H
#define TRACKER_H

#include <QPoint>
#include <QString>
#include <QMap>
#include <QSet>

class Trackable;

class Tracker
{
public:
    enum TargetProperty {
        ALL = 1,
        POSITION = 2,
        REL_POSITION = 3,
        ROTATION = 4
    };

    enum { TrackerType = 1 };
    virtual int trackerType() const { return TrackerType; }

public:
    Tracker();
    virtual ~Tracker();

    virtual bool link(Trackable* target, int target_prop = ALL);
    virtual bool grab(Trackable* target, int target_prop = ALL);
    virtual bool manipulate(Trackable* target, QString const& key);

    virtual bool removeLink(Trackable* target, int target_prop = -1);
    virtual bool removeGrab(Trackable* target, int target_prop = -1);
    virtual bool removeManipulate(Trackable* target, QString const& key = "");
    virtual void removeAllTracking(Trackable* target = nullptr);

    virtual const QList<int> supportedTargetProperties() const;

    void beginModify();
    void endModify();

    const QString& getName() const;
    const QPointF& getPosition() const;
    const QPointF& getRelativePosition() const;
    qreal getRotation() const;

    virtual void set(const Tracker* other);
    virtual void setName(const QString&);
    virtual void setPosition(const QPointF&);
    virtual void setRelativePosition(const QPointF&);
    virtual void setRotation(qreal);

    bool isLinked(Trackable* target, int target_prop = -1) const;
    bool isGrabbed(Trackable* target, int target_prop = -1) const;
    bool isManipulated(Trackable* target, QString const& key = "") const;

    const QSet<Trackable *> linkTargets() const;
    const QSet<Trackable *> grabTargets() const;
    const QSet<Trackable *> manipulationTargets() const;
    virtual const QSet<Trackable*> targets() const;

    bool getCleanByModelEnabled() const;
    void setCleanByModelEnabled(bool state);

protected:
    bool internalLink(Trackable* target, int target_prop);
    bool internalGrab(Trackable* target, int target_prop);
    bool internalManipulate(Trackable* target, const QString& key);
    void internalRemoveLink(Trackable* target, int target_prop);
    void internalRemoveGrab(Trackable* target, int target_prop);
    void internalRemoveManipulate(Trackable* target, const QString& key);

    virtual void propertiesUpdatedEvent();
    virtual void positionUpdatedEvent();
    virtual void relativePositionUpdatedEvent();
    virtual void rotationUpdatedEvent();

    void updateTargets(int target_property = -1);

    QString name_;

    QPointF position_;
    QPointF relative_position_;
    qreal rotation_;

    QMap<int, QSet<Trackable*>> links_;
    QMap<int, QSet<Trackable*>> grabs_;
    QMap<QString, QSet<Trackable*>> manipulations_;
    bool bulk_update_;
    bool cleaned_by_model_;
};

#endif // TRACKER_H
