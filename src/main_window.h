#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QList>
#include <QMap>
#include <QAction>

#include "companion_widget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void toggleFullScreen(bool);

private:
    void initCentralWidget();
    void initActions();
    void initMenuBar();
    void initStatusBar();

    CompanionWidget* companion_;
    QMap<QString, QList<QAction*> > actions_;
};

#endif // MAIN_WINDOW_H
