#include "tuio_control_panel.h"

#include <QJsonDocument>

#include <QHBoxLayout>

#include <QGraphicsView>
#include <QPainter>
#include <QtMath>
#include <QBrush>
#include <QMouseEvent>
#include <QMessageBox>
#include <QHeaderView>
#include <QGroupBox>
#include <QPointF>

#include "image/image_item.h"
#include "image/interactive/interactive_image.h"
#include "tuio_token_item.h"
#include "tuio_cursor_item.h"
#include "tuio_blob_item.h"
#include "resources/lib.h"
#include "tracking/tracker.h"

TuioControlPanel::TuioControlPanel(QWidget *parent)
    : QWidget(parent)
    , marker_list_()
    , token_list_()
    , blob_list_()
    , token_registry_(0)
    , host_name_(0)
    , host_submit_(0)
    , host_label_(0)
    , view_(0)
    , tuio_handler_(0)
    , cursor_table_(0)
    , token_table_(0)
    , blob_table_(0)
    , menu_bar_(0)
    , main_splitter_(0)
{
    setWindowTitle("Tuio Control Panel");
    //setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);

    initTuio(QHostAddress::Any, 3333);
    initWidgets();
    initActions();
    initLayout();
}


TuioControlPanel::~TuioControlPanel()
{
    for(auto m : marker_list_.values()) {
        view_->scene()->removeItem(m);
        delete m;
    }
    for(auto t : token_list_.values()) {
        view_->scene()->removeItem(t);
        delete t;
    }
    for(auto b: blob_list_.values()) {
        view_->scene()->removeItem(b);
    }
    if(token_registry_)
        token_registry_->deleteLater();
}

void TuioControlPanel::onNewHostName()
{
    QList<QString> host_str(host_name_->text().split(":"));
    if(host_str.size() != 2) {
        QMessageBox b;
        b.setText(tr("Invalid address for TUIO server"));
        b.setInformativeText(tr("Address should be of format <xxx.xxx.xxx.xxx:port>"));
        b.setStandardButtons(QMessageBox::Ok);
        b.setDefaultButton(QMessageBox::Ok);
        b.exec();
        return;
    }

    QHostAddress addr(host_str[0]);
    unsigned port = (unsigned) host_str[1].toInt();
    initTuio(addr, port);

    QMessageBox b;
    b.setText(tr("New TUIO host set"));
    b.setInformativeText(addr.toString() + ":" + QString::number(port));
    b.setStandardButtons(QMessageBox::Ok);
    b.setDefaultButton(QMessageBox::Ok);
    b.exec();
}

void TuioControlPanel::onCursorChanged(int id, TuioCursorTableModel::CursorChange c)
{
    if(c == TuioCursorTableModel::CURSOR_ADDED || c == TuioCursorTableModel::CURSOR_UPDATED) {
        // add or update cursor marker
        QTuioCursor cursor = tuio_handler_->getCursorModel()->getCursor(id);
        if(cursor.id() == -1)
            return;
        if(!marker_list_.contains(id)) {
            auto marker = new TuioCursorItem(cursor.id());
            marker->setX(view_->sceneRect().width() * cursor.x());
            marker->setY(view_->sceneRect().height() * cursor.y());
            marker_list_[id] = marker;
            view_->addItem(marker);
        } else {
            marker_list_[id]->setX(view_->sceneRect().width() * cursor.x());
            marker_list_[id]->setY(view_->sceneRect().height() * cursor.y());
        }
    }
    else if(c == TuioCursorTableModel::CURSOR_REMOVED) {
        // delete cursor marker
        if(marker_list_.contains(id)) {
            view_->scene()->removeItem(marker_list_[id]);
            delete marker_list_[id];
            marker_list_.remove(id);
        }
    }
}

void TuioControlPanel::onTokenChanged(int id, TuioTokenTableModel::TokenChange c)
{
    if(c == TuioTokenTableModel::TOKEN_ADDED || c == TuioTokenTableModel::TOKEN_UPDATED) {
        // add or update token marker
        QTuioToken token = tuio_handler_->getTokenModel()->getToken(id);
        if(token.id() == -1)
            return;
        if(!token_list_.contains(id)) {
            auto marker = new TuioTokenItem(token.id(), token.classId());
            //marker->setBrush(QBrush(Qt::red));
            marker->setX(view_->sceneRect().width() * token.x());
            marker->setY(view_->sceneRect().height()* token.y());
            marker->setRotation(qRadiansToDegrees(token.angle()));
            token_list_[id] = marker;
            view_->addItem(marker);
            foreach(auto it, view_->scene()->selectedItems())
                it->setSelected(false);
            marker->setSelected(true);
        } else {
            token_list_[id]->setX(view_->sceneRect().width() * token.x());
            token_list_[id]->setY(view_->sceneRect().height() * token.y());
            token_list_[id]->setRotation(qRadiansToDegrees(token.angle()));
        }
        updateTrackedData(token);
    }
    else if(c == TuioTokenTableModel::TOKEN_REMOVED) {
        // delete token marker
        if(token_list_.contains(id)) {
            view_->scene()->removeItem(token_list_[id]);
            delete token_list_[id];
            token_list_.remove(id);
        }
    }
}

void TuioControlPanel::onBlobChanged(int id, TuioBlobTableModel::BlobChange c)
{
    if(c == TuioBlobTableModel::BLOB_ADDED || c == TuioBlobTableModel::BLOB_UPDATED) {
        // add or update cursor marker
        QTuioBlob blob = tuio_handler_->getBlobModel()->getBlob(id);
        if(blob.id() == -1)
            return;
        if(!blob_list_.contains(id)) {
            auto marker = new TuioBlobItem(blob.id());
            float center_x = view_->sceneRect().width() * blob.x();
            float center_y = view_->sceneRect().height() * blob.y();

            marker->setX(center_x);
            marker->setY(center_y);

            float width = view_->sceneRect().width() * blob.width();
            float height = view_->sceneRect().height() * blob.height();
            marker->setBoundingRect(QSize(width, height));

            marker->setRotation(qRadiansToDegrees(blob.angle()));

            blob_list_[id] = marker;
            view_->addItem(marker);
        } else {
            blob_list_[id]->setX(view_->sceneRect().width() * blob.x());
            blob_list_[id]->setY(view_->sceneRect().height() * blob.y());
        }
    }
    else if(c == TuioBlobTableModel::BLOB_REMOVED) {
        // delete cursor marker
        if(blob_list_.contains(id)) {
            view_->scene()->removeItem(blob_list_[id]);
            delete blob_list_[id];
            blob_list_.remove(id);
        }
    }

}

void TuioControlPanel::onTokenFieldSelected(const QModelIndex &idx)
{
    Q_UNUSED(idx)
    qDebug().nospace() << Q_FUNC_INFO << " @ line " << __LINE__;
    qDebug() << "  > " << "TODO: implement";
}

void TuioControlPanel::onRegisterCursorTracker()
{
    QMessageBox b;
    b.setText(QString(Q_FUNC_INFO) + " @ line " + QString::number(__LINE__));
    b.setInformativeText("TODO register cursor tracker");
    b.setModal(false);
    b.exec();
}

void TuioControlPanel::onRegisterTokenTracker()
{
    if(token_registry_ == 0) {
        token_registry_ = new RegisterTokenDialog;
        foreach(auto it, view_->scene()->selectedItems()) {
            TuioTokenItem* tti = qgraphicsitem_cast<TuioTokenItem*>(it);
            if(!tti)
                continue;
            QTuioToken token = tuio_handler_->getTokenModel()->getToken(tti->getID());
            if(token_registry_)
                token_registry_->setToken(token);
        }
        connect(token_registry_, &RegisterTokenDialog::trackerCreated,
                this, &TuioControlPanel::onTrackerAdded);
    }
    if(token_registry_->isVisible()) {
        token_registry_->raise();
        token_registry_->activateWindow();
    }
    else {
        token_registry_->show();
    }
}

void TuioControlPanel::onRegisterBlobTracker()
{
    QMessageBox b;
    b.setText(QString(Q_FUNC_INFO) + " @ line " + QString::number(__LINE__));
    b.setInformativeText("TODO register blob tracker");
    b.setModal(false);
    b.exec();
}

void TuioControlPanel::onSceneSelectionChanged()
{
    if(view_->scene()->selectedItems().size() == 0) {
        if(token_registry_)
            token_registry_->setToken(QTuioToken());
        return;
    }
    foreach(auto it, view_->scene()->selectedItems()) {
        TuioTokenItem* tti = qgraphicsitem_cast<TuioTokenItem*>(it);
        if(!tti)
            continue;
        QTuioToken token = tuio_handler_->getTokenModel()->getToken(tti->getID());
        if(token_registry_)
            token_registry_->setToken(token);
    }
}

void TuioControlPanel::onTrackerAdded(const TuioTokenTracker &tracker)
{
    Tracker* new_tracker = new TuioTokenTracker;
    new_tracker->set(&tracker);
    Resources::Lib::TRACKER_MODEL->addTracker(new_tracker);
}

void TuioControlPanel::updateTrackedData(const QTuioToken &token)
{
    TuioTokenTracker temp_tracker;
    foreach(auto tracker, Resources::Lib::TRACKER_MODEL->getTrackers()) {
        if(tracker->trackerType() == TuioTokenTracker::TrackerType) {
            temp_tracker.set(tracker);
            if(temp_tracker.isCompatible(token)) {
                temp_tracker.set(token);
                Resources::Lib::TRACKER_MODEL->updateTracker(&temp_tracker);
            }
        }
    }
}

void TuioControlPanel::initTuio(const QHostAddress &ip, unsigned port)
{
    if(tuio_handler_)
        tuio_handler_->deleteLater();
    tuio_handler_ = new TuioModelHandler(ip, port, this);
    connect(tuio_handler_->getCursorModel(), &TuioCursorTableModel::cursorChanged,
            this, &TuioControlPanel::onCursorChanged);
    connect(tuio_handler_->getTokenModel(), &TuioTokenTableModel::tokenChanged,
            this, &TuioControlPanel::onTokenChanged);
    connect(tuio_handler_->getBlobModel(), &TuioBlobTableModel::blobChanged,
            this, &TuioControlPanel::onBlobChanged);
    if(cursor_table_)
        cursor_table_->setModel(tuio_handler_->getCursorModel());
    if(token_table_)
        token_table_->setModel(tuio_handler_->getTokenModel());
    if(blob_table_)
    {
        qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
        qDebug() << "  >" << "blob model";
        qDebug() << "  >" << tuio_handler_->getBlobModel();
        qDebug() << "  >" << "blob talbe";
        qDebug() << "  >" << blob_table_;


        blob_table_->setModel(tuio_handler_->getBlobModel());
    }

}

void TuioControlPanel::initWidgets()
{
    menu_bar_ = new QMenuBar(this);
    main_splitter_ = new QSplitter(Qt::Horizontal, this);

    view_ =  new TuioGraphicsView(this);
    view_->setRenderHints(QPainter::Antialiasing);
    connect(view_->scene(), &QGraphicsScene::selectionChanged,
            this, &TuioControlPanel::onSceneSelectionChanged);

    host_name_ = new QLineEdit(this);
    host_name_->setPlaceholderText("255.255.255.255:3333");

    host_submit_ = new QPushButton(tr("Submit"), this);
    connect(host_submit_, &QPushButton::clicked,
            this, &TuioControlPanel::onNewHostName);

    host_label_ = new QLabel(tr("TUIO Host"), this);

    cursor_table_ = new QTableView(this);
    cursor_table_->setModel(tuio_handler_->getCursorModel());
    cursor_table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    cursor_table_->horizontalHeader()->setStretchLastSection(true);
    cursor_table_->setDragEnabled(true);
    cursor_table_->horizontalHeader()->setSectionsMovable(true);

    token_table_ = new QTableView(this);
    token_table_->setModel(tuio_handler_->getTokenModel());
    token_table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    token_table_->horizontalHeader()->setStretchLastSection(true);
    token_table_->setDragEnabled(true);
    token_table_->horizontalHeader()->setSectionsMovable(true);
    connect(token_table_, &QTableView::clicked,
            this, &TuioControlPanel::onTokenFieldSelected);

    blob_table_ = new QTableView(this);
    blob_table_->setModel(tuio_handler_->getBlobModel());
    blob_table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    blob_table_->horizontalHeader()->setStretchLastSection(true);
    blob_table_->setDragEnabled(true);
    blob_table_->horizontalHeader()->setSectionsMovable(true);
}

void TuioControlPanel::initActions()
{
    QAction* show_data_view = new QAction(tr("Data View"), this);
    show_data_view->setCheckable(true);
    show_data_view->setChecked(false);
    show_data_view->setShortcut(QKeySequence("Alt+0"));
    connect(show_data_view, &QAction::toggled,
            this, [=](bool state) {
        if(state && main_splitter_->sizes()[0] == 0) {
            main_splitter_->setSizes(QList<int>() << 100 << 300);
        }
        else if(!state && main_splitter_->sizes()[0] > 0) {
            main_splitter_->setSizes(QList<int>() << 0 << 100);
        }
    });

    QAction* register_cursor = new QAction(tr("New Cursor Tracker..."), this);
    register_cursor->setShortcut(QKeySequence("Alt+C"));
    connect(register_cursor, &QAction::triggered,
            this, &TuioControlPanel::onRegisterCursorTracker);

    QAction* register_token = new QAction(tr("New Token Tracker..."), this);
    register_token->setShortcut(QKeySequence("Alt+T"));
    connect(register_token, &QAction::triggered,
            this, &TuioControlPanel::onRegisterTokenTracker);

    QAction* register_blob = new QAction(tr("New Blob Tracker..."), this);
    register_blob->setShortcut(QKeySequence("Alt+B"));
    connect(register_blob, &QAction::triggered,
            this, &TuioControlPanel::onRegisterBlobTracker);

    QMenu* tool_menu = new QMenu(tr("Tools"), this);
    tool_menu->addActions(QList<QAction*>()
        << show_data_view
    );

    QMenu* add_menu = new QMenu(tr("Add"), this);
    add_menu->addActions(QList<QAction*>()
        << register_cursor
        << register_token
        << register_blob
    );

    menu_bar_->addMenu(tool_menu);
    menu_bar_->addMenu(add_menu);
}

void TuioControlPanel::initLayout()
{
    QHBoxLayout* bottom = new QHBoxLayout;
    bottom->addWidget(host_label_,-1);
    bottom->addWidget(host_name_, 100);
    bottom->addWidget(host_submit_,-1);
    bottom->setContentsMargins(5, 5, 5, 5);
    bottom->setSpacing(5);

    QSplitter* monitor_splitter = new QSplitter(Qt::Vertical, this);
    QGroupBox* token_box = new QGroupBox(tr("Tokens"), this);
    token_box->setLayout(new QHBoxLayout);
    token_box->layout()->addWidget(token_table_);
    QGroupBox* cursor_box = new QGroupBox(tr("Cursors"), this);
    cursor_box->setLayout(new QHBoxLayout);
    cursor_box->layout()->addWidget(cursor_table_);
    QGroupBox* blob_box = new QGroupBox(tr("Blobs"), this);
    blob_box->setLayout(new QHBoxLayout);
    blob_box->layout()->addWidget(blob_table_);
    monitor_splitter->addWidget(token_box);
    monitor_splitter->addWidget(cursor_box);
    monitor_splitter->addWidget(blob_box);

    QGroupBox* box_left = new QGroupBox(this);
    box_left->setLayout(new QHBoxLayout);
    box_left->layout()->addWidget(monitor_splitter);

    QGroupBox* box_right = new QGroupBox(this);
    box_right->setLayout(new QHBoxLayout);
    box_right->layout()->addWidget(view_);

    main_splitter_->addWidget(box_left);
    main_splitter_->addWidget(box_right);
    main_splitter_->setSizes(QList<int>() << 0 << 100);
    main_splitter_->setContentsMargins(5,0,5,0);

    QVBoxLayout *root = new QVBoxLayout;
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);
    root->addWidget(menu_bar_,-1);
    root->addWidget(main_splitter_);
    root->addLayout(bottom,-1);

    setLayout(root);
}
