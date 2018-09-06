#ifndef INTERACTIVE_IMAGE_SHAPE_WIDGET_H
#define INTERACTIVE_IMAGE_SHAPE_WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>

#include "interactive_image_shape.h"
#include "tracking/tracker_picker.h"

class InteractiveImageShapeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InteractiveImageShapeWidget(QWidget *parent = nullptr);
    explicit InteractiveImageShapeWidget(InteractiveImageShape* shape, QWidget *parent = nullptr);
    virtual ~InteractiveImageShapeWidget();

    void setShape(InteractiveImageShape* token);
    InteractiveImageShape* getShape() const;

    void toggleCollapse();

signals:

public slots:
    void onSave();
    void onDelete();
    void onTrackerAddButtonClicked();

private slots:
    void onCollapseTriggered();

private:
    void hideCollapsibleWidgets();
    void showCollapsibleWidgets();
    void contentsModifiedEvent();
    void updateUI();
    void blockContentModifiedEvent(bool blocked);

    void initWidgets();
    void initLayout();

    InteractiveImageShape* shape_;
    QLabel* title_label_;
    QLabel* name_label_;
    QLineEdit* name_edit_;
    QPushButton* delete_button_;
    QPushButton* save_button_;
    QPushButton* collapse_button_;
    QPushButton* tracker_add_button_;
    QCheckBox* uncover_box_;
    QCheckBox* fog_visibility_box_;

    QList<QWidget*> collapsible_widgets_;
    QList<QWidget*> content_modifying_widgets_;
    bool block_modified_event_;
};

#endif // INTERACTIVE_IMAGE_SHAPE_WIDGET_H
