#ifndef WIDGET_LIST_VIEW_H
#define WIDGET_LIST_VIEW_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>

class WidgetListView : public QScrollArea
{
    Q_OBJECT
public:
    explicit WidgetListView(QWidget *parent = nullptr);
    virtual ~WidgetListView();

    virtual void addWidget(QWidget* w);
    virtual void removeWidget(QWidget* w);

    const QList<QWidget*> getWidgets() const;
signals:

protected slots:
    void onItemDestroyed(QObject* o);

protected:
    void init();

    QVBoxLayout* scroll_layout_;
    QWidget* contents_;
};

#endif // WIDGET_LIST_VIEW_H
