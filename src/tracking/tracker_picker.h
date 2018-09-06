#ifndef TRACKER_PICKER_H
#define TRACKER_PICKER_H

#include <QComboBox>

class TrackerPicker : public QComboBox
{
    Q_OBJECT
public:
    TrackerPicker(QWidget* parent = 0);
    TrackerPicker(int type_filter, QWidget* parent = 0);

    void setCurrentTracker(const QString&);

    void setTrackerTypeFilter(int);
    int getTrackerTypeFilter() const;

protected slots:
    void onTrackerAdded(const QString&);
    void onTrackerRemoved(const QString&);

private:
    void init();

    int type_filter_;
};

#endif // TRACKER_PICKER_H
