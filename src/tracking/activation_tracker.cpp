#include "activation_tracker.h"
#include "trackable.h"

#include <QDebug>

ActivationTracker::ActivationTracker()
    : Tracker()
    , active_state_(false)
{}

ActivationTracker::~ActivationTracker()
{}

bool ActivationTracker::link(Trackable *target, int target_prop)
{
    return Tracker::link(target, target_prop);
}

bool ActivationTracker::grab(Trackable *target, int target_prop)
{
    Q_UNUSED(target);
    Q_UNUSED(target_prop);
    return false;
}

bool ActivationTracker::manipulate(Trackable *target, const QString &key)
{
    return key.compare("active_state") == 0 ? link(target, ACTIVE_STATE) : false;
}

const QList<int> ActivationTracker::supportedTargetProperties() const
{
    QList<int> p;
    p.append(ACTIVE_STATE);
    return p;
}

void ActivationTracker::setActiveState(bool enabled)
{
    if(active_state_ == enabled)
        return;
    active_state_ = enabled;
    updateTargets(ACTIVE_STATE);
}

bool ActivationTracker::getActiveState() const
{
    return active_state_;
}
