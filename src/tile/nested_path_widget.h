#ifndef NESTED_PATH_WIDGET_H
#define NESTED_PATH_WIDGET_H

#include <QWidget>

#include <QLabel>
#include <QPushButton>
#include <QStack>
#include <QMap>
#include <QGraphicsScene>
#include <QPropertyAnimation>
#include <QTimer>

#include "misc/progress_button.h"

class NestedPathWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NestedPathWidget(QWidget *parent = nullptr);

    void setPathText(const QStack<QGraphicsScene*>& scene_stack,
                       const QMap<QGraphicsScene*, QString> scene_names);
    const QString getPathText() const;

signals:
    void backButtonClicked();

public slots:

private:
    void startBackProgress();
    void resetBackProgress();

    void dragEnterEvent(QDragEnterEvent* e);
    void dropEvent(QDropEvent *e);
    void dragLeaveEvent(QDragLeaveEvent* e);

    void initWidgets();
    void initLayout();

    ProgressButton* back_button_;
    QLabel* path_label_;
    QTimer back_timer_;
    QPropertyAnimation* progress_animation_;
};

#endif // NESTED_PATH_WIDGET_H
