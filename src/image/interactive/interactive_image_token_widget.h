#ifndef INTERACTIVE_IMAGE_TOKEN_WIDGET_H
#define INTERACTIVE_IMAGE_TOKEN_WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include <QCheckBox>

#include "interactive_image_token.h"
#include "tracking/tracker_picker.h"

class InteractiveImageTokenWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InteractiveImageTokenWidget(QWidget *parent = nullptr);
    explicit InteractiveImageTokenWidget(InteractiveImageToken* token, QWidget *parent = nullptr);
    virtual ~InteractiveImageTokenWidget();

    void setToken(InteractiveImageToken* token);
    InteractiveImageToken* getToken() const;

    void toggleCollapse();

signals:

public slots:
    void onSave();
    void onDelete();

private slots:
    void onCollapseTriggered();
    void onChooseColor();

private:
    void hideCollapsibleWidgets();
    void showCollapsibleWidgets();
    void contentsModifiedEvent();
    void updateUI();
    void blockContentModifiedEvent(bool blocked);

    void initWidgets();
    void initLayout();

    InteractiveImageToken* token_;
    QLabel* title_label_;
    QLabel* name_label_;
    QLineEdit* name_edit_;
    QLabel* tracker_label_;
    TrackerPicker* tracker_picker_;
    QPushButton* delete_button_;
    QPushButton* save_button_;
    QPushButton* collapse_button_;
    QRadioButton* link_select_;
    QRadioButton* grab_select_;
    QSlider *grab_radius_slider_;
    QSlider *uncover_radius_slider_;
    QLabel *token_color_label_;
    QPushButton *token_color_button_;
    QList<QWidget*> collapsible_widgets_;
    QList<QWidget*> content_modifying_widgets_;
    bool block_modified_event_;
};

#endif // INTERACTIVE_IMAGE_TOKEN_WIDGET_H
