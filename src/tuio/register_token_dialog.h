#ifndef REGISTER_TOKEN_DIALOG_H
#define REGISTER_TOKEN_DIALOG_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>

#include "qtuiotoken_p.h"

#include "tracking/tuio_token_tracker.h"

class RegisterTokenDialog : public QWidget
{
    Q_OBJECT
public:
    RegisterTokenDialog(QWidget* parent = nullptr);

    void setToken(const QTuioToken&);
    const QTuioToken& getToken() const;

signals:
    void trackerCreated(const TuioTokenTracker&);

public slots:

private slots:
    void onSubmit();

private:
    void checkSubmitEnabled();
    void tokenChangedEvent();
    void disableSubmit();
    void enableSubmit();

    void initWidgets();
    void initLayout();

    QTuioToken token_;
    TuioTokenTracker tracker_;
    QLabel* l_text_;
    QRadioButton* rb_track_id_;
    QRadioButton* rb_track_class_id_;
    QCheckBox* cb_flip_x_;
    QCheckBox* cb_flip_y_;
    QLabel* l_name_;
    QLineEdit* le_name_;
    QPushButton* pb_submit_;
};

#endif // REGISTER_TOKEN_DIALOG_H
