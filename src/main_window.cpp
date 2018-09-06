#include "main_window.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QProgressBar>
#include <QKeySequence>

#include "resources/lib.h"
#include <qtuiocursor_p.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , companion_(0)
    , actions_()
{
    setWindowTitle("Companion");
    initCentralWidget();
    initActions();
    initMenuBar();
    initStatusBar();
}

MainWindow::~MainWindow()
{}

void MainWindow::toggleFullScreen(bool enable_full)
{
    if(enable_full)
        this->showFullScreen();
    else
        this->showMaximized();
}

void MainWindow::initCentralWidget()
{
    companion_ = new CompanionWidget(this);
    setCentralWidget(companion_);
}

void MainWindow::initActions()
{
    QList<QAction*> window_actions;

    // Full Screen action
    window_actions.append(new QAction(tr("Full Screen"), this));
    window_actions.back()->setToolTip(tr("Show application in Full Screen mode"));
    window_actions.back()->setCheckable(true);
    window_actions.back()->setShortcut(QKeySequence(tr("Ctrl+Shift+F11")));

    connect(window_actions.back(), SIGNAL(toggled(bool)),
            this, SLOT(toggleFullScreen(bool)));

    actions_[tr("Window")] = window_actions;
}

void MainWindow::initMenuBar()
{
    // add actions from DsaMediaControlKit
    menuBar()->addActions(companion_->getMenu()->actions());

    // add MainWindow actions
    foreach(QString const& menu_name, actions_.keys()) {
        // create new menu bar item for each action category
        QMenu* new_menu = menuBar()->addMenu(menu_name);
        // append all actions to menu category
        foreach(QAction* act, actions_[menu_name])
            new_menu->addAction(act);
    }
}

void MainWindow::initStatusBar()
{
    statusBar()->addWidget(companion_->getProgressBar(), 1);
}
