#include "tracker_picker.h"

#include "resources/lib.h"

TrackerPicker::TrackerPicker(QWidget* parent)
    : QComboBox(parent)
    , type_filter_(-1)
{
    init();
}

TrackerPicker::TrackerPicker(int type_filter, QWidget *parent)
    : QComboBox(parent)
    , type_filter_(type_filter)
{
    init();
}

void TrackerPicker::setCurrentTracker(const QString &n)
{
    for(int i = 0; i < count(); ++i) {
        if(itemText(i).compare(n) == 0) {
            setCurrentIndex(i);
            return;
        }
    }
}

void TrackerPicker::setTrackerTypeFilter(int tracker_type)
{
    type_filter_ = tracker_type;
    clear();
    foreach(auto t, Resources::Lib::TRACKER_MODEL->getTrackers())
        onTrackerAdded(t->getName());
}

int TrackerPicker::getTrackerTypeFilter() const
{
    return type_filter_;
}

void TrackerPicker::onTrackerAdded(const QString &n)
{
    if(type_filter_ != -1) {
        if(Resources::Lib::TRACKER_MODEL->getTracker(n)->trackerType() != type_filter_)
            return;
    }
    addItem(n);
}

void TrackerPicker::onTrackerRemoved(const QString &n)
{
    for(int i = 0; i < count(); ++i) {
        if(itemText(i).compare(n) == 0) {
            removeItem(i);
            return;
        }
    }
}

void TrackerPicker::init()
{
    addItem("");
    foreach(auto t, Resources::Lib::TRACKER_MODEL->getTrackers())
        onTrackerAdded(t->getName());
    connect(Resources::Lib::TRACKER_MODEL, &TrackerTableModel::trackerAdded,
            this, &TrackerPicker::onTrackerAdded);
    connect(Resources::Lib::TRACKER_MODEL, &TrackerTableModel::trackerRemoved,
            this, &TrackerPicker::onTrackerRemoved);
}
