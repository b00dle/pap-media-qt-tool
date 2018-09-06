#include "register_token_dialog.h"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QDebug>

RegisterTokenDialog::RegisterTokenDialog(QWidget* parent)
    : QWidget(parent)
    , token_()
    , tracker_()
    , l_text_(0)
    , rb_track_id_(0)
    , rb_track_class_id_(0)
    , cb_flip_x_(0)
    , cb_flip_y_(0)
    , l_name_(0)
    , le_name_(0)
    , pb_submit_(0)
{
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
    setWindowTitle(tr("Register Token Tracker"));
    initWidgets();
    initLayout();
}

void RegisterTokenDialog::setToken(const QTuioToken &t)
{
    token_ = t;
    tokenChangedEvent();
}

const QTuioToken &RegisterTokenDialog::getToken() const
{
    return token_;
}

void RegisterTokenDialog::onSubmit()
{
    int id = -1;
    int class_id = -1;
    if(rb_track_class_id_->isChecked())
        class_id = tracker_.getClassId();
    else
        id = tracker_.getId();
    tracker_ = TuioTokenTracker(class_id, id);
    tracker_.setName(le_name_->text());
    tracker_.setXFlipped(cb_flip_x_->checkState() == Qt::Checked);
    tracker_.setYFlipped(cb_flip_y_->checkState() == Qt::Checked);
    tracker_.set(token_);
    emit trackerCreated(tracker_);
    if(isVisible())
        hide();
}

void RegisterTokenDialog::checkSubmitEnabled()
{
    if(le_name_->text().size() == 0 || !tracker_.isValid())
        return disableSubmit();
    enableSubmit();
}

void RegisterTokenDialog::tokenChangedEvent()
{
    tracker_ = TuioTokenTracker(token_.classId(), token_.id());
    tracker_.set(token_);
    rb_track_id_->setText(tr("id [") + QString::number(tracker_.getId()) + "]");
    rb_track_class_id_->setText(tr("class id [") + QString::number(tracker_.getClassId()) + "]");
    checkSubmitEnabled();
}

void RegisterTokenDialog::initWidgets()
{
    l_text_ = new QLabel(tr("Put a new Token onto the tracked area, or select one from the control panel."
                            "\nConfigure and choose a unique name for the tracker, then click submit."), this);

    rb_track_id_ = new QRadioButton(tr("id [-1]"), this);
    rb_track_class_id_ = new QRadioButton(tr("class id [-1]"), this);
    cb_flip_x_ = new QCheckBox(tr("flip X"), this);
    cb_flip_x_->setChecked(false);
    cb_flip_y_ = new QCheckBox(tr("flip Y"), this);
    cb_flip_y_->setChecked(false);
    l_name_ = new QLabel(tr("Tracker Name"), this);

    le_name_ = new QLineEdit(this);
    le_name_->setPlaceholderText("<unique name>");
    connect(le_name_, &QLineEdit::textChanged,
            this, [=](){checkSubmitEnabled();});

    pb_submit_ = new QPushButton(tr("submit"), this);
    pb_submit_->setEnabled(false);
    connect(pb_submit_, &QPushButton::clicked,
            this, &RegisterTokenDialog::onSubmit);
}

void RegisterTokenDialog::initLayout()
{
    QGroupBox* radio_prop_group = new QGroupBox(tr("Tracked ID"), this);
    radio_prop_group->setLayout(new QVBoxLayout);
    radio_prop_group->layout()->addWidget(rb_track_id_);
    radio_prop_group->layout()->addWidget(rb_track_class_id_);
    rb_track_class_id_->setChecked(true);

    QGroupBox* check_group = new QGroupBox(tr("Modify Tracked Position"), this);
    check_group->setLayout(new QVBoxLayout);
    check_group->layout()->addWidget(cb_flip_x_);
    check_group->layout()->addWidget(cb_flip_y_);

    QHBoxLayout* mid = new QHBoxLayout;
    mid->addWidget(radio_prop_group);
    mid->addWidget(check_group);
    mid->setContentsMargins(0,0,0,0);

    QHBoxLayout* bottom = new QHBoxLayout;
    bottom->addWidget(l_name_,-1);
    bottom->addWidget(le_name_, 100);
    bottom->addWidget(pb_submit_,-1);
    bottom->setContentsMargins(5, 5, 5, 5);
    bottom->setSpacing(5);

    QVBoxLayout* root = new QVBoxLayout;
    root->addWidget(l_text_);
    root->addLayout(mid);
    root->addLayout(bottom);

    setLayout(root);
}

void RegisterTokenDialog::disableSubmit()
{
    l_text_->setText(tr("Put a new Token onto the tracked area, or select one from the control panel."
                        "\nConfigure and choose a unique name for the tracker, then click submit."));
    pb_submit_->setEnabled(false);
}

void RegisterTokenDialog::enableSubmit()
{
    l_text_->setText(tr("Click Submit to create tracker '") + le_name_->text() + tr("'"));
    pb_submit_->setEnabled(true);
}
