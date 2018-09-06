#ifndef TUIO_TRACKER_H
#define TUIO_TRACKER_H

#include "tracker.h"

#include "qtuiotoken_p.h"

class TuioTokenTracker : public Tracker
{
public:
    enum { TrackerType = Tracker::TrackerType + 2 };
    virtual int trackerType() const { return TrackerType; }

public:
    TuioTokenTracker();
    TuioTokenTracker(int class_id, int id = -1);
    virtual ~TuioTokenTracker();

    virtual void set(const Tracker* other);

    virtual void setRelativePosition(QPointF const&);
    virtual const QList<int> supportedTargetProperties() const;

    bool set(QTuioToken const&);

    int getId() const;
    int getClassId() const;

    void setXFlipped(bool enabled);
    bool getXFlipped() const;
    void setYFlipped(bool enabled);
    bool getYFlipped() const;

    bool isValid() const;
    bool isCompatible(QTuioToken const&);

protected:
    QPointF const flip(const QPointF&) const;

    int id_;
    int class_id_;
    bool flip_x_;
    bool flip_y_;
};

#endif // TUIO_TRACKER_H
