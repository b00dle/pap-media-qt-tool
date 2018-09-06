#include "tracker_picker_dialog.h"
#include "resources/lib.h"

#include <QVBoxLayout>

TrackerPickerDialog::TrackerPickerDialog(QWidget* parent)
    : QDialog(parent)
    , picker_(0)
    , label_(0)
    , submit_button_(0)
    , cancel_button_(0)
{
    initWidgets();
    initLayout();
}

TrackerPickerDialog::TrackerPickerDialog(int type_filter, QWidget *parent)
    : QDialog(parent)
    , picker_(0)
    , label_(0)
    , submit_button_(0)
    , cancel_button_(0)
{
    initWidgets();
    initLayout();
    Q_UNUSED(type_filter);
    //picker_->setTrackerTypeFilter(type_filter);
}

TrackerPickerDialog::~TrackerPickerDialog()
{}

Tracker *TrackerPickerDialog::getTracker() const
{
    if(!Resources::Lib::TRACKER_MODEL->hasTracker(picker_->currentText()))
        return 0;
    return Resources::Lib::TRACKER_MODEL->getTracker(picker_->currentText());
}

void TrackerPickerDialog::setTracker(Tracker *t)
{
    if(!Resources::Lib::TRACKER_MODEL->hasTracker(t))
        return;
    picker_->setCurrentTracker(t->getName());
}

int TrackerPickerDialog::getTrackerTypeFilter() const
{
    return picker_->getTrackerTypeFilter();
}

void TrackerPickerDialog::setTrackerTypeFilter(int filter)
{
    picker_->setTrackerTypeFilter(filter);
}

void TrackerPickerDialog::initWidgets()
{
    picker_ = new TrackerPicker(this);

    label_ = new QLabel(tr("Choose a Tracker from the drop down list."), this);

    submit_button_ = new QPushButton(tr("submit"), this);
    connect(submit_button_, &QPushButton::clicked,
            this, &TrackerPickerDialog::accept);

    cancel_button_ = new QPushButton(tr("cancel"), this);
    connect(cancel_button_, &QPushButton::clicked,
            this, &TrackerPickerDialog::reject);
}

void TrackerPickerDialog::initLayout()
{
    QVBoxLayout* layout = new QVBoxLayout;

    QHBoxLayout* bottom = new QHBoxLayout;
    bottom->addStretch(2);
    bottom->addWidget(submit_button_);
    bottom->addWidget(cancel_button_);
    bottom->setContentsMargins(0,0,0,0);

    layout->addWidget(label_);
    layout->addWidget(picker_);
    layout->addLayout(bottom);

    setLayout(layout);
}
