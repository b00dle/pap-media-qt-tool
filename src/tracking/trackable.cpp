#include "trackable.h"

#include <QDebug>

#include "tracker.h"
#include "resources/lib.h"

Trackable::Trackable()
    : links_()
    , grabs_()
    , manipulations_()
    , allow_forced_tracker_change_(true)
    , avoid_remove_cycles_(false)
    , trackable_name_("")
{}

Trackable::~Trackable()
{
    removeAllTracking();
}

bool Trackable::registerLink(Tracker *tracker, int target_prop)
{
    if(!prepareRegistration(target_prop))
        return false;
    links_[target_prop] = tracker;
    updateLinkFromTracker(tracker, target_prop);
    return true;
}

bool Trackable::registerGrab(Tracker *tracker, int target_prop)
{
    if(!prepareRegistration(target_prop))
        return false;
    grabs_[target_prop] = tracker;
    updateGrabFromTracker(tracker, target_prop);
    return true;
}

bool Trackable::registerManipulate(Tracker *tracker, const QString &key)
{
    if(key.compare("link") == 0)
        return registerLink(tracker, Tracker::ALL);
    else if(key.compare("grab") == 0)
        return registerGrab(tracker, Tracker::ALL);
    else if(!prepareRegistration(key))
        return false;
    manipulations_[key] = tracker;
    updateFromTracker(tracker, key);
    return true;
}

bool Trackable::updateGrabFromTracker(Tracker *tracker, int target_prop)
{
    if(!hasGrab(target_prop) || (grabs_[target_prop] != tracker))
        return false;
    return true;
}

bool Trackable::updateLinkFromTracker(Tracker *tracker, int target_prop)
{
    if(!hasLink(target_prop) || (links_[target_prop] != tracker))
        return false;
    return true;
}

bool Trackable::removeLink(int target_prop)
{
    if(!hasLink(target_prop)) {
        return false;
    }
    else if(!avoid_remove_cycles_) {
        avoid_remove_cycles_ = true;
        if(target_prop == -1) {
            foreach(auto p, supportedTargetProperties()) {
                if(hasLink(p))
                    links_[p]->removeLink(this, p);
            }
        }
        else {
            links_[target_prop]->removeLink(this, target_prop);
        }
        avoid_remove_cycles_ = false;
    }
    else {
        if(target_prop == -1) {
            foreach(auto p, supportedTargetProperties())
                removeLink(p);
        }
        else {
            links_[target_prop] = nullptr;
        }
    }
    return true;
}

bool Trackable::removeGrab(int target_prop)
{
    if(!hasGrab(target_prop)) {
        return false;
    }
    else if(!avoid_remove_cycles_) {
        avoid_remove_cycles_ = true;
        if(target_prop == -1) {
            foreach(auto p, supportedTargetProperties()) {
                if(hasGrab(p))
                    grabs_[p]->removeGrab(this, p);
            }
        }
        else {
            grabs_[target_prop]->removeGrab(this, target_prop);
        }
        avoid_remove_cycles_ = false;
    }
    else {
        if(target_prop == -1) {
            foreach(auto p, supportedTargetProperties()) {
                if(hasGrab(p))
                    removeGrab(p);
            }
        }
        else {
            grabs_[target_prop] = nullptr;
        }
    }
    return true;
}

bool Trackable::removeManipulate(const QString &key)
{
    if(!hasManipulate(key)) {
        return false;
    }
    if(key.size() == 0) {
        removeLink();
        removeGrab();
        foreach(auto k, manipulations_.keys()) {
            if(manipulations_[k])
                removeManipulate(k);
        }
    }
    else if(key.compare("link") == 0) {
        return removeLink();
    }
    else if(key.compare("grab") == 0) {
        return removeGrab();
    }
    else if(!avoid_remove_cycles_) {
        avoid_remove_cycles_ = true;
        manipulations_[key]->removeManipulate(this, key);
        avoid_remove_cycles_ = false;
    }
    else {
        manipulations_[key] = nullptr;
    }
    return true;
}

bool Trackable::removeAllTracking()
{
    return removeManipulate();
}

bool Trackable::updateFromTracker(Tracker *tracker, QString const& key)
{
    if(key.compare("link") == 0)
        return updateLinkFromTracker(tracker, Tracker::ALL);
    else if(key.compare("grab") == 0)
        return updateGrabFromTracker(tracker, Tracker::ALL);
    return false;
}

const QList<int> Trackable::supportedTargetProperties() const
{
    QList<int> p;
    p.append(Tracker::ALL);
    p.append(Tracker::POSITION);
    p.append(Tracker::REL_POSITION);
    p.append(Tracker::ROTATION);
    return p;
}

bool Trackable::hasLink(int prop)
{
    if(prop == -1) {
        foreach(auto t, links_.values()) {
            if(t)
                return true;
        }
        return false;
    }
    return links_.contains(prop) && links_[prop];
}

bool Trackable::hasGrab(int prop)
{
    if(prop == -1) {
        foreach(auto t, grabs_.values()) {
            if(t)
                return true;
        }
        return false;
    }
    return grabs_.contains(prop) && grabs_[prop];
}

bool Trackable::hasManipulate(const QString &key)
{
    if(key.size() == 0) {
        if(hasLink())
            return true;
        if(hasGrab())
            return true;
        foreach(auto t, manipulations_.values()) {
            if(t)
                return true;
        }
        return false;
    }
    else if(key.compare("link") == 0) {
        return hasLink();
    }
    else if(key.compare("grab") == 0) {
        return hasGrab();
    }
    return manipulations_.contains(key) && manipulations_[key];
}

void Trackable::setTrackableName(const QString &name)
{
    trackable_name_ = name;
}

const QString &Trackable::getTrackableName() const
{
    return trackable_name_;
}

bool Trackable::getForcedTrackerChangeEnabled() const
{
    return allow_forced_tracker_change_;
}

void Trackable::setForcedTrackerChangeEnabled(bool val)
{
    allow_forced_tracker_change_ = val;
}

bool Trackable::ensureNotManipulated(int target_prop)
{
    if(hasLink(target_prop)) {
        if(allow_forced_tracker_change_)
            removeLink(target_prop);
        else
            return false;
    }
    else if(hasGrab(target_prop)) {
        if(allow_forced_tracker_change_)
            removeGrab(target_prop);
        else
            return false;
    }
    return true;
}

bool Trackable::prepareRegistration(int target_prop)
{
    if(!ensureNotManipulated(target_prop) || !supportedTargetProperties().contains(target_prop))
        return false;

    if(target_prop == Tracker::ALL) {
        if(!ensureNotManipulated(Tracker::POSITION))
            return false;
        if(!ensureNotManipulated(Tracker::REL_POSITION))
            return false;
        if(!ensureNotManipulated(Tracker::ROTATION))
            return false;
    }
    else if(target_prop == Tracker::POSITION || target_prop == Tracker::REL_POSITION) {
        if(!ensureNotManipulated(Tracker::POSITION))
            return false;
        if(!ensureNotManipulated(Tracker::REL_POSITION))
            return false;
    }
    else if(target_prop == Tracker::ROTATION){
        if(!ensureNotManipulated(Tracker::ROTATION))
            return false;
    }

    return true;
}

bool Trackable::prepareRegistration(const QString &key)
{
    if(hasManipulate(key)) {
        if(allow_forced_tracker_change_)
            removeManipulate(key);
        else
            return false;
    }
    return true;
}
