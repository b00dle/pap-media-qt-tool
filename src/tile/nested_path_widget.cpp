#include "nested_path_widget.h"

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QDebug>

#include "resources/lib.h"

NestedPathWidget::NestedPathWidget(QWidget *parent)
    : QWidget(parent)
    , back_button_(nullptr)
    , path_label_(nullptr)
    , back_timer_()
    , progress_animation_(nullptr)
{
    setAcceptDrops(true);
    initWidgets();
    initLayout();
}

void NestedPathWidget::setPathText(const QStack<QGraphicsScene *> &scene_stack, const QMap<QGraphicsScene *, QString> scene_names)
{
    QString path = "";
    int i = 0;
    foreach(auto s, scene_stack) {
        if(i != 0)
            path += " > ";
        if(i == scene_stack.size() - 1)
            path += "<b>";
        path += scene_names[s];
        if(i == scene_stack.size() - 1)
            path += "</b>";
        ++i;
    }
    path_label_->setText(path);
    adjustSize();
}

const QString NestedPathWidget::getPathText() const
{
    return path_label_->text();
}

void NestedPathWidget::startBackProgress()
{
    back_timer_.start(1000);
    if(progress_animation_ == nullptr) {
        progress_animation_ = new QPropertyAnimation(back_button_, "progress");
        progress_animation_->setStartValue(0.1);
        progress_animation_->setEndValue(100.0);
        progress_animation_->setDuration(1000);
        progress_animation_->setEasingCurve(QEasingCurve::Linear);
        progress_animation_->start();
    }
}

void NestedPathWidget::resetBackProgress()
{
    back_timer_.stop();
    if(progress_animation_) {
        progress_animation_->stop();
        progress_animation_->deleteLater();
        progress_animation_ = nullptr;
        back_button_->setProgress(-1.0);
    }
}

void NestedPathWidget::dragEnterEvent(QDragEnterEvent *e)
{
    QWidget::dragEnterEvent(e);
    e->setDropAction(Qt::CopyAction);
    e->accept();
    startBackProgress();
}

void NestedPathWidget::dropEvent(QDropEvent *e)
{
    QWidget::dropEvent(e);
    e->setDropAction(Qt::CopyAction);
    e->accept();
    resetBackProgress();
}

void NestedPathWidget::dragLeaveEvent(QDragLeaveEvent *e)
{
    QWidget::dragLeaveEvent(e);
    e->accept();
    resetBackProgress();
}

void NestedPathWidget::initWidgets()
{
    back_button_ = new ProgressButton(this);
    back_button_->setFixedSize(50,50);
    back_button_->setMask(QRegion(back_button_->geometry(), QRegion::Ellipse));
    back_button_->setIcon(QIcon(*Resources::Lib::PX_BACK_BUTTON));
    QSize button_size(back_button_->geometry().size());
    button_size.setHeight(button_size.height()*0.66);
    button_size.setWidth(button_size.height());
    back_button_->setIconSize(button_size);
    back_button_->setStyleSheet("QPushButton {"
                                "   border: none; "
                                "   opacity: 0.8; "
                                "}"
                                ""
                                "QPushButton:pressed {"
                                "   background-color: rgba(50, 152, 253, 40);"
                                "}"
                                "");
    connect(back_button_, &ProgressButton::clicked,
            this, [=]() {
        resetBackProgress();
        emit backButtonClicked();
    });

    path_label_ = new QLabel(this);

    back_timer_.setSingleShot(true);
    connect(&back_timer_, &QTimer::timeout,
            back_button_, &ProgressButton::click);
}

void NestedPathWidget::initLayout()
{
    QHBoxLayout* layout = new QHBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(back_button_);
    layout->addWidget(path_label_);
    setLayout(layout);
}
