#ifndef ACTIVATION_TRACKER_H
#define ACTIVATION_TRACKER_H

#include "tracker.h"

class ActivationTracker : public Tracker
{
public:
    enum TargetProperty {
        ACTIVE_STATE = 77
    };

    enum { TrackerType = Tracker::TrackerType + 3 };
    virtual int trackerType() const { return TrackerType; }
public:
    ActivationTracker();
    virtual ~ActivationTracker();

    virtual bool link(Trackable* target, int target_prop = ACTIVE_STATE);
    virtual bool grab(Trackable* target, int target_prop = ALL);
    virtual bool manipulate(Trackable* target, QString const& key = "active_state");

    virtual const QList<int> supportedTargetProperties() const;

    void setActiveState(bool enabled);
    bool getActiveState() const;

protected:
    bool active_state_;
};

#endif // ACTIVATION_TRACKER_H
