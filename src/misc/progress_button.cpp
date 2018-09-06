#include "progress_button.h"

#include <QPainter>
#include <QDebug>

ProgressButton::ProgressButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
    , progress_(-1.0)
{}

ProgressButton::ProgressButton(QWidget *parent)
    : QPushButton(parent)
{}

void ProgressButton::setProgress(qreal v)
{
    progress_ = v;
    if(progress_ >= 100.0)
        progress_ = 0.0;
    update();
}

qreal ProgressButton::getProgress() const
{
    return progress_;
}

void ProgressButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);
    if(progress_ < 0.0 || progress_ > 100.0)
        return;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect progress_rect(geometry());
    progress_rect.setWidth(progress_rect.width()*(progress_ / 100.0));
    painter.fillRect(progress_rect, QBrush(QColor(0,255,0,150)));
}
