#include "tracker.h"

#include "trackable.h"

Tracker::Tracker()
    : position_()
    , relative_position_()
    , rotation_()
    , links_()
    , grabs_()
    , manipulations_()
    , bulk_update_(false)
    , cleaned_by_model_(true)
{}

Tracker::~Tracker()
{
    removeAllTracking();
}

bool Tracker::link(Trackable *target, int target_prop)
{
    if(!target->getForcedTrackerChangeEnabled()) {
        if(isLinked(target, target_prop) || isGrabbed(target, target_prop))
            return false;
    }
    if(!supportedTargetProperties().contains(target_prop))
        return false;
    // check linkability
    bool can_link = false;
    if(target_prop == ALL) {
        can_link = true;
        if(isLinked(target))
            removeLink(target);
    }
    else {
        can_link = true;
    }
    /*else if(target_prop == POSITION || target_prop == REL_POSITION) {
        can_link = (!isLinked(target, POSITION) || !isLinked(target, REL_POSITION)) &&
                (!isGrabbed(target, POSITION) || !isGrabbed(target, REL_POSITION));
    }
    else if(target_prop == ROTATION) {
        can_link = true;
    }*/
    // link if linkable
    if(can_link)
        return internalLink(target, target_prop);
    return false;
}

bool Tracker::grab(Trackable *target, int target_prop)
{
    if(!target->getForcedTrackerChangeEnabled()) {
        if(isLinked(target, target_prop) || isGrabbed(target, target_prop))
            return false;
    }
    if(!supportedTargetProperties().contains(target_prop))
        return false;
    // check grabbability
    bool can_grab = false;
    if(target_prop == ALL) {
        can_grab = !isLinked(target);
        if(isGrabbed(target))
            removeGrab(target);
    }
    else {
        can_grab = true;
    }
//    else if(target_prop == POSITION || target_prop == REL_POSITION) {
//        can_grab = (!isLinked(target, POSITION) || !isLinked(target, REL_POSITION)) &&
//                (!isGrabbed(target, POSITION) || !isGrabbed(target, REL_POSITION));
//    }
//    else if(target_prop == ROTATION) {
//        can_grab = true;
//    }
    // link if grabbable
    if(can_grab)
        return internalGrab(target, target_prop);
    return false;
}

bool Tracker::manipulate(Trackable *target, const QString &key)
{
    if(!target->getForcedTrackerChangeEnabled()) {
        if(isManipulated(target, key))
            return false;
    }
    return internalManipulate(target, key);
}

bool Tracker::removeLink(Trackable *target, int target_prop)
{
    if(!isLinked(target, target_prop))
        return false;
    internalRemoveLink(target, target_prop);
    return true;
}

bool Tracker::removeGrab(Trackable *target, int target_prop)
{
    if(!isGrabbed(target, target_prop))
        return false;
    internalRemoveGrab(target, target_prop);
    return true;
}

bool Tracker::removeManipulate(Trackable *target, const QString &key)
{
    if(!isManipulated(target, key))
        return false;
    internalRemoveManipulate(target, key);
    return true;
}

void Tracker::removeAllTracking(Trackable* target)
{
    if(target == nullptr) {
        foreach(auto t, targets())
            removeManipulate(t);
        return;
    }
    removeManipulate(target);
}

const QList<int> Tracker::supportedTargetProperties() const
{
    QList<int> p;
    p.append(ALL);
    p.append(POSITION);
    p.append(REL_POSITION);
    p.append(ROTATION);
    return p;
}

void Tracker::beginModify()
{
    bulk_update_ = true;
}

void Tracker::endModify()
{
    bulk_update_ = false;
    updateTargets();
}

const QString &Tracker::getName() const
{
    return name_;
}

const QPointF &Tracker::getPosition() const
{
    return position_;
}

const QPointF &Tracker::getRelativePosition() const
{
    return relative_position_;
}

qreal Tracker::getRotation() const
{
    return rotation_;
}

void Tracker::set(const Tracker *other)
{
    setName(other->getName());
    setPosition(other->getPosition());
    setRelativePosition(other->getRelativePosition());
    setRotation(other->getRotation());
}

void Tracker::setName(const QString &n)
{
    name_ = n;
}

void Tracker::setPosition(const QPointF &p)
{
    position_ = p;
    if(!bulk_update_)
        positionUpdatedEvent();
}

void Tracker::setRelativePosition(const QPointF &p)
{
    relative_position_ = p;
    if(!bulk_update_)
        relativePositionUpdatedEvent();
}

void Tracker::setRotation(qreal r)
{
    rotation_ = r;
    if(!bulk_update_)
        rotationUpdatedEvent();
}

bool Tracker::isLinked(Trackable *target, int target_prop) const
{
    if(target_prop == -1) {
        foreach(auto p, links_.keys()) {
            if(links_[p].contains(target))
                return true;
        }
        return false;
    }
    return (links_.contains(ALL) && links_[ALL].contains(target)) ||
            (links_.contains(target_prop) && links_[target_prop].contains(target));
}

bool Tracker::isGrabbed(Trackable *target, int target_prop) const
{
    if(target_prop == -1) {
        foreach(auto p, grabs_.keys()) {
            if(grabs_[p].contains(target))
                return true;
        }
        return false;
    }
    return (grabs_.contains(ALL) && grabs_[ALL].contains(target)) ||
            (grabs_.contains(target_prop) && grabs_[target_prop].contains(target));
}

bool Tracker::isManipulated(Trackable *target, const QString &key) const
{
    if(key.size() == 0) {
        if(isLinked(target))
            return true;
        if(isGrabbed(target))
            return true;
        foreach(auto k, manipulations_.keys()) {
            if(manipulations_[k].contains(target))
                return true;
        }
        return false;
    }
    if(key.compare("link") == 0)
        return isLinked(target);
    else if(key.compare("grab") == 0)
        return isGrabbed(target);
    return manipulations_.contains(key) && manipulations_[key].contains(target);
}

const QSet<Trackable *> Tracker::linkTargets() const
{
    QSet<Trackable*> t;
    foreach(auto k, links_.keys())
        t.unite(links_[k]);
    return t;
}

const QSet<Trackable *> Tracker::grabTargets() const
{
    QSet<Trackable*> t;
    foreach(auto k, grabs_.keys())
        t.unite(grabs_[k]);
    return t;
}

const QSet<Trackable *> Tracker::manipulationTargets() const
{
    QSet<Trackable*> t;
    t.unite(linkTargets());
    t.unite(grabTargets());
    foreach(auto k, manipulations_.keys())
        t.unite(manipulations_[k]);
    return t;
}

const QSet<Trackable *> Tracker::targets() const
{
    return manipulationTargets();
}

bool Tracker::getCleanByModelEnabled() const
{
    return cleaned_by_model_;
}

void Tracker::setCleanByModelEnabled(bool state)
{
    cleaned_by_model_ = state;
}

bool Tracker::internalLink(Trackable *target, int target_prop)
{
    if(!target->registerLink(this, target_prop))
        return false;
    if(!links_.contains(target_prop))
        links_[target_prop] = QSet<Trackable*>();
    links_[target_prop].insert(target);
    return true;
}

bool Tracker::internalGrab(Trackable *target, int target_prop)
{
    if(!target->registerGrab(this, target_prop))
        return false;
    if(!grabs_.contains(target_prop))
        grabs_[target_prop] = QSet<Trackable*>();
    grabs_[target_prop].insert(target);
    return true;
}

bool Tracker::internalManipulate(Trackable *target, const QString& key)
{
    if(key.compare("link") == 0)
        return internalLink(target, ALL);
    else if(key.compare("grab") == 0)
        return internalGrab(target, ALL);
    if(!target->registerManipulate(this, key))
        return false;
    if(!manipulations_.contains(key))
        manipulations_[key] = QSet<Trackable*>();
    manipulations_[key].insert(target);
    return true;
}

void Tracker::internalRemoveLink(Trackable *target, int target_prop)
{
    if(target_prop == -1) {
        foreach(auto p, supportedTargetProperties()) {
            if(isLinked(target, p))
                internalRemoveLink(target, p);
        }
        return;
    }
    if(links_.contains(target_prop) && links_[target_prop].contains(target)) {
        if(target->removeLink(target_prop))
            links_[target_prop].remove(target);
    }
}

void Tracker::internalRemoveGrab(Trackable *target, int target_prop)
{
    if(target_prop == -1) {
        foreach(auto p, supportedTargetProperties()) {
            if(isGrabbed(target, p))
                internalRemoveGrab(target, p);
        }
        return;
    }
    if(grabs_.contains(target_prop) && grabs_[target_prop].contains(target)) {
        if(target->removeGrab(target_prop))
            grabs_[target_prop].remove(target);
    }
}

void Tracker::internalRemoveManipulate(Trackable *target, const QString &key)
{
    if(key.size() == 0) {
        internalRemoveLink(target, -1);
        internalRemoveGrab(target, -1);
        foreach(auto k, manipulations_.keys()) {
            if(isManipulated(target, k))
                internalRemoveManipulate(target, k);
        }
        return;
    }
    else if(key.compare("link") == 0) {
        return internalRemoveLink(target, -1);
    }
    else if(key.compare("grab") == 0) {
        return internalRemoveGrab(target, -1);
    }
    if(manipulations_.contains(key) && manipulations_[key].contains(target)) {
        if(target->removeManipulate(key))
            manipulations_[key].remove(target);
    }
}

void Tracker::propertiesUpdatedEvent()
{
    updateTargets();
}

void Tracker::positionUpdatedEvent()
{
    updateTargets(POSITION);
}

void Tracker::relativePositionUpdatedEvent()
{
    updateTargets(REL_POSITION);
}

void Tracker::rotationUpdatedEvent()
{
    updateTargets(ROTATION);
}

void Tracker::updateTargets(int target_property)
{
    if(target_property == -1) {
        foreach(auto p, supportedTargetProperties()) {
            foreach(auto t, targets()) {
                if(isLinked(t, p))
                    t->updateLinkFromTracker(this, p);
                if(isGrabbed(t, p))
                    t->updateGrabFromTracker(this, p);
            }
        }
        foreach(auto k, manipulations_.keys()) {
            foreach(auto t, targets()) {
                if(isManipulated(t, k))
                    t->updateFromTracker(this, k);
            }
        }
    }
    else {
        foreach(auto t, targets()) {
            if(isLinked(t, target_property))
                t->updateLinkFromTracker(this, target_property);
            if(isGrabbed(t, target_property))
                t->updateGrabFromTracker(this, target_property);
        }
    }
}


