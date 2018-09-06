#include "interactive_image_shape_widget.h"

#include <QGroupBox>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QDebug>

#include "resources/lib.h"

InteractiveImageShapeWidget::InteractiveImageShapeWidget(QWidget *parent)
    : QWidget(parent)
    , shape_(0)
    , title_label_(0)
    , name_label_(0)
    , name_edit_(0)
    , delete_button_(0)
    , save_button_(0)
    , collapse_button_(0)
    , tracker_add_button_(0)
    , uncover_box_(0)
    , fog_visibility_box_(0)
    , collapsible_widgets_()
    , content_modifying_widgets_()
    , block_modified_event_(false)
{
    initWidgets();
    initLayout();
    hideCollapsibleWidgets();
}

InteractiveImageShapeWidget::InteractiveImageShapeWidget(InteractiveImageShape *shape, QWidget *parent)
    : QWidget(parent)
    , shape_(shape)
    , title_label_(0)
    , name_label_(0)
    , name_edit_(0)
    , delete_button_(0)
    , save_button_(0)
    , collapse_button_(0)
    , tracker_add_button_(0)
    , uncover_box_(0)
    , fog_visibility_box_(0)
    , collapsible_widgets_()
    , content_modifying_widgets_()
    , block_modified_event_(false)
{
    initWidgets();
    initLayout();
    blockContentModifiedEvent(shape->getName().size() > 0);
    updateUI();
    hideCollapsibleWidgets();
    blockContentModifiedEvent(false);
    connect(shape_, &InteractiveImageShape::destroyed,
            this, &InteractiveImageShapeWidget::deleteLater);
}

InteractiveImageShapeWidget::~InteractiveImageShapeWidget()
{}

void InteractiveImageShapeWidget::setShape(InteractiveImageShape *shape)
{
    if(shape_) {
        disconnect(shape_, &InteractiveImageShape::destroyed,
                   this, &InteractiveImageShapeWidget::deleteLater);
    }
    shape_ = shape;
    connect(shape_, &InteractiveImageShape::destroyed,
            this, &InteractiveImageShapeWidget::deleteLater);
    updateUI();
}

InteractiveImageShape *InteractiveImageShapeWidget::getShape() const
{
    return shape_;
}

void InteractiveImageShapeWidget::onSave()
{
    if(shape_->getName().compare(name_edit_->text()) != 0)
        shape_->setName(name_edit_->text());
    if(uncover_box_->isChecked() != shape_->getUncoverEnabled())
        shape_->setUncoverEnabled(uncover_box_->isChecked());
    if(fog_visibility_box_->isChecked() != shape_->isVisibleInFog())
        shape_->setFogVisibility(fog_visibility_box_->isChecked());
    title_label_->setText(name_edit_->text());
    save_button_->setEnabled(false);
    updateUI();
}

void InteractiveImageShapeWidget::onDelete()
{
    QMessageBox b;
    b.setText(tr("You are about to delete a shape named '") + title_label_->text() + tr("'."));
    b.setInformativeText(tr("Do you wish to proceed?"));
    b.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    b.setDefaultButton(QMessageBox::No);
    b.setWindowTitle(tr("Delete Shape"));
    if(b.exec() == QMessageBox::Yes) {
        shape_->deleteLater();
    }
}

void InteractiveImageShapeWidget::onTrackerAddButtonClicked()
{
    if(save_button_->isEnabled()) {
        QMessageBox b;
        b.setText(tr("You have unsaved changes for this shape."));
        b.setInformativeText(tr("To avoid unexpected behavior, please save them before proceeding"));
        b.setStandardButtons(QMessageBox::Ok);
        b.exec();
        return;
    }

    if(tracker_add_button_->text().startsWith("add", Qt::CaseInsensitive)) {
        if(shape_->getName().size() == 0) {
            QMessageBox b;
            b.setText(tr("Cannot add tracker."));
            b.setInformativeText(tr("Please assign a name for your shape before adding a tracker from it."));
            b.setStandardButtons(QMessageBox::Ok);
            b.exec();
            return;
        }
        if(Resources::Lib::TRACKER_MODEL->hasTracker(shape_) ||
           Resources::Lib::TRACKER_MODEL->hasTracker(shape_->getName()))
        {
            QMessageBox b;
            b.setText(tr("This tracker already exists."));
            b.setInformativeText(tr("You might have added this tracker under a different name, "
                                    "or a tracker with a similar name already exists"));
            b.setStandardButtons(QMessageBox::Ok);
            b.setDefaultButton(QMessageBox::Ok);
            b.exec();
            return;
        }
        Resources::Lib::TRACKER_MODEL->addTracker(shape_);
        tracker_add_button_->setText("remove activation tracker");
    }
    else {
        QMessageBox b;
        b.setText(tr("You are about to delete '")+shape_->getName()+tr("' as a tracker."));
        b.setInformativeText(tr("Do you wish to proceed?"));
        b.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        b.setDefaultButton(QMessageBox::No);
        if(b.exec() != QMessageBox::Yes)
            return;
        Resources::Lib::TRACKER_MODEL->removeTracker(shape_);
        tracker_add_button_->setText("add activation tracker");
    }
}

void InteractiveImageShapeWidget::onCollapseTriggered()
{
    if(collapse_button_->text().compare("show more") == 0) {
        collapse_button_->setText(tr("show less"));
        showCollapsibleWidgets();
    }
    else {
        collapse_button_->setText(tr("show more"));
        hideCollapsibleWidgets();
    }
}

void InteractiveImageShapeWidget::hideCollapsibleWidgets()
{
    foreach(auto w, collapsible_widgets_)
        w->hide();
}

void InteractiveImageShapeWidget::showCollapsibleWidgets()
{
    foreach(auto w, collapsible_widgets_)
        w->show();
}

void InteractiveImageShapeWidget::contentsModifiedEvent()
{
    if(save_button_->isEnabled())
        return;
    save_button_->setEnabled(true);
    title_label_->setText(title_label_->text()+"*");
}

void InteractiveImageShapeWidget::updateUI()
{
    if(shape_) {
        if(shape_->getName().size() > 0) {
            title_label_->setText(shape_->getName());
        }
        else {
            title_label_->setText(tr("UNNAMED SHAPE"));
        }
        name_edit_->setText(shape_->getName());
        uncover_box_->setChecked(shape_->getUncoverEnabled());
        fog_visibility_box_->setChecked(shape_->isVisibleInFog());
        if(!Resources::Lib::TRACKER_MODEL->hasTracker(shape_))
            tracker_add_button_->setText("add activation tracker");
        else
            tracker_add_button_->setText("remove activation tracker");
    }
    else {
        title_label_->setText(tr("UNNAMED SHAPE"));
        name_edit_->setText("");
        uncover_box_->setChecked(false);
        fog_visibility_box_->setChecked(false);
        tracker_add_button_->setText("add activation tracker");
    }
}

void InteractiveImageShapeWidget::blockContentModifiedEvent(bool blocked)
{
    if(blocked == block_modified_event_)
        return;
    block_modified_event_ = blocked;
    foreach(auto w, content_modifying_widgets_)
        w->blockSignals(block_modified_event_);
}

void InteractiveImageShapeWidget::initWidgets()
{
    title_label_ = new QLabel(tr("UNNAMED SHAPE"), this);
    QFont f(title_label_->font());
    f.setPointSize(f.pointSize()*1.5);
    title_label_->setFont(f);

    name_label_ = new QLabel(tr("Name"), this);
    name_edit_ = new QLineEdit(this);
    name_edit_->setPlaceholderText(tr("<name here>"));
    connect(name_edit_, &QLineEdit::textChanged,
            this, [=](){contentsModifiedEvent();});
    content_modifying_widgets_.append(name_edit_);

    delete_button_ = new QPushButton(tr("delete"), this);
    connect(delete_button_, &QPushButton::clicked,
            this, &InteractiveImageShapeWidget::onDelete);

    save_button_ = new QPushButton(tr("save"), this);
    save_button_->setEnabled(false);
    connect(save_button_, &QPushButton::clicked,
            this, &InteractiveImageShapeWidget::onSave);

    collapse_button_ = new QPushButton(tr("show more"), this);
    connect(collapse_button_, &QPushButton::clicked,
            this, &InteractiveImageShapeWidget::onCollapseTriggered);

    uncover_box_ = new QCheckBox(tr("is fog uncover shape"), this);
    connect(uncover_box_, &QCheckBox::clicked,
            this, [=](){contentsModifiedEvent();});
    content_modifying_widgets_.append(uncover_box_);

    fog_visibility_box_ = new QCheckBox(tr("merged outline is visible in fog"), this);
    connect(fog_visibility_box_, &QCheckBox::clicked,
            this, [=](){contentsModifiedEvent();});
    content_modifying_widgets_.append(fog_visibility_box_);

    tracker_add_button_ = new QPushButton(tr("add activation tracker"), this);
    connect(tracker_add_button_, &QPushButton::clicked,
            this, &InteractiveImageShapeWidget::onTrackerAddButtonClicked);

    collapsible_widgets_.append(name_label_);
    collapsible_widgets_.append(name_edit_);
    collapsible_widgets_.append(uncover_box_);
    collapsible_widgets_.append(fog_visibility_box_);
    collapsible_widgets_.append(tracker_add_button_);
}

void InteractiveImageShapeWidget::initLayout()
{
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    QVBoxLayout* group_layout = new QVBoxLayout;
    QGroupBox* group = new QGroupBox(this);
    group->setLayout(group_layout);
    layout->addWidget(group,10);
    layout->addWidget(collapse_button_,-1);

    setLayout(layout);
    QSizePolicy p(sizePolicy());
    p.setVerticalPolicy(QSizePolicy::Fixed);
    setSizePolicy(p);

    QHBoxLayout* header_layout = new QHBoxLayout;
    header_layout->addWidget(title_label_);
    header_layout->addStretch(4);
    header_layout->addWidget(save_button_, -1);
    header_layout->addWidget(delete_button_, -1);

    QHBoxLayout* name_layout = new QHBoxLayout;
    name_layout->addWidget(name_label_,1);
    name_layout->addWidget(name_edit_,3);

    group_layout->addLayout(header_layout, -1);
    group_layout->addLayout(name_layout);
    group_layout->addWidget(uncover_box_);
    group_layout->addWidget(fog_visibility_box_);
    group_layout->addWidget(tracker_add_button_,-1);
}
