#ifndef PROGRESS_BUTTON_H
#define PROGRESS_BUTTON_H

#include <QPushButton>

class ProgressButton : public QPushButton
{
    Q_OBJECT

    Q_PROPERTY(qreal progress MEMBER progress_ READ getProgress WRITE setProgress)

public:
    ProgressButton(const QString& text, QWidget* parent = 0);
    ProgressButton(QWidget* parent = 0);

    void setProgress(qreal v);
    qreal getProgress() const;

protected:
    void paintEvent(QPaintEvent* event);

    qreal progress_;
};

#endif // PROGRESS_BUTTON_H
