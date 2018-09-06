#ifndef TRACKER_PICKER_DIALOG_H
#define TRACKER_PICKER_DIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>

#include "tracker.h"
#include "tracker_picker.h"

class TrackerPickerDialog: public QDialog
{
public:
    TrackerPickerDialog(QWidget* parent = 0);
    TrackerPickerDialog(int type_filter, QWidget* parent = 0);
    virtual ~TrackerPickerDialog();

    Tracker* getTracker() const;
    void setTracker(Tracker*);

    int getTrackerTypeFilter() const;
    void setTrackerTypeFilter(int);

private:
    void initWidgets();
    void initLayout();

    TrackerPicker* picker_;
    QLabel* label_;
    QPushButton* submit_button_;
    QPushButton* cancel_button_;
};

#endif // TRACKER_PICKER_DIALOG_H
