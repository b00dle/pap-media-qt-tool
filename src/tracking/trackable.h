#ifndef TRACKABLE_H
#define TRACKABLE_H

#include <QString>
#include <QMap>

class Tracker;

class Trackable
{
public:
    Trackable();
    virtual ~Trackable();

    virtual bool registerLink(Tracker* tracker, int target_prop);
    virtual bool registerGrab(Tracker* tracker, int target_prop);
    virtual bool registerManipulate(Tracker* tracker, QString const& key);

    virtual bool updateLinkFromTracker(Tracker* tracker, int target_prop) = 0;
    virtual bool updateGrabFromTracker(Tracker* tracker, int target_prop) = 0;
    virtual bool updateFromTracker(Tracker* tracker, QString const& key);

    virtual bool removeLink(int target_prop = -1);
    virtual bool removeGrab(int target_prop = -1);
    virtual bool removeManipulate(QString const& k = "");
    virtual bool removeAllTracking();

    virtual const QList<int> supportedTargetProperties() const;

    bool hasLink(int prop = -1);
    bool hasGrab(int prop = -1);
    bool hasManipulate(const QString& key = "");

    virtual void setTrackableName(const QString& name);
    const QString& getTrackableName() const;

    bool getForcedTrackerChangeEnabled() const;
    void setForcedTrackerChangeEnabled(bool);

protected:
    bool ensureNotManipulated(int target_prop);
    bool prepareRegistration(int target_prop);
    bool prepareRegistration(const QString&);

    QMap<int,Tracker*> links_;
    QMap<int,Tracker*> grabs_;
    QMap<QString,Tracker*> manipulations_;
    bool allow_forced_tracker_change_;
    bool avoid_remove_cycles_;
    QString trackable_name_;
};

#endif // TRACKABLE_H
