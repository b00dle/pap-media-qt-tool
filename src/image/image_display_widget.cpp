#include "image_display_widget.h"

#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "view.h"
#include "misc/widget_list_view.h"
#include "misc/container.h"
#include "interactive/interactive_image.h"
#include "interactive/interactive_image_token_widget.h"
#include "interactive/interactive_image_shape_widget.h"

ImageDisplayWidget::ImageDisplayWidget(QWidget *parent)
    : QWidget(parent)
    , menu_bar_(0)
    , view_menu_(0)
    , view_(0)
    , token_config_list_(0)
    , shape_config_list_(0)
    , main_splitter_(0)
{
    initWidgets();
    initMenu();
    initLayout();
}

Image::View *ImageDisplayWidget::getView() const
{
    return view_;
}

void ImageDisplayWidget::popOpen()
{
    if(isHidden())
        showNormal();
    else
        show();
    raise();
    activateWindow();
}

void ImageDisplayWidget::onItemSet()
{
    view_menu_->clear();
    view_menu_->addActions(view_->getMenuBarExtension()->actions());
    view_menu_->addSeparator();
    QAction* zoom_in = view_menu_->addAction(tr("Zoom In"));
    connect(zoom_in, &QAction::triggered,
            this, &ImageDisplayWidget::onZoomIn);
    QAction* zoom_out = view_menu_->addAction(tr("Zoom Out"));
    connect(zoom_out, &QAction::triggered,
            this, &ImageDisplayWidget::onZoomOut);
    auto img = qgraphicsitem_cast<InteractiveImage*>(view_->getItem());
    if(img) {
        connect(img, &InteractiveImage::tokenAdded,
                this, &ImageDisplayWidget::onTokenAdded);
        connect(img, &InteractiveImage::shapeAdded,
                this, &ImageDisplayWidget::onShapeAdded);
    }
    else {
        removeAllTokenConfigs();
    }
}

void ImageDisplayWidget::onToggleFullscreen()
{
    if (windowState().testFlag(Qt::WindowFullScreen)) {
        menu_bar_->show();
        showNormal();
    }
    else {
        menu_bar_->hide();
        showFullScreen();
    }
}

void ImageDisplayWidget::onZoomIn()
{
    view_->scale(1.1, 1.1);
}

void ImageDisplayWidget::onZoomOut()
{
    view_->scale(0.9, 0.9);
}

void ImageDisplayWidget::onTokenAdded(InteractiveImageToken *token)
{
    token_config_list_->addWidget(new InteractiveImageTokenWidget(token));
}

void ImageDisplayWidget::onShapeAdded(InteractiveImageShape* shape)
{
    shape_config_list_->addWidget(new InteractiveImageShapeWidget(shape));
}

void ImageDisplayWidget::removeAllTokenConfigs()
{
    QList<QWidget*> configs = token_config_list_->getWidgets();
    while(configs.size() > 0) {
        token_config_list_->removeWidget(configs[0]);
        configs[0]->deleteLater();
        configs.pop_front();
    }
}

void ImageDisplayWidget::removeAllShapeConfigs()
{
    QList<QWidget*> configs = shape_config_list_->getWidgets();
    while(configs.size() > 0) {
        shape_config_list_->removeWidget(configs[0]);
        configs[0]->deleteLater();
        configs.pop_front();
    }
}

void ImageDisplayWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
        case '-':
            onZoomOut();
            break;
        case '+':
            onZoomIn();
            break;
        case ' ':
            onToggleFullscreen();
            break;
    }
    QWidget::keyPressEvent(event);
}

void ImageDisplayWidget::initWidgets()
{
    view_ = new Image::View(this);
    menu_bar_ = new QMenuBar(this);
    token_config_list_ = new WidgetListView(this);
    shape_config_list_ = new WidgetListView(this);
    main_splitter_ = new QSplitter(Qt::Horizontal, this);
    //main_splitter_->setHandleWidth(30);

    connect(view_, SIGNAL(itemSet()),
            this, SLOT(onItemSet()));
}

void ImageDisplayWidget::initMenu()
{
    view_menu_ = menu_bar_->addMenu(tr("View"));
    QMenu* window_menu = new QMenu(tr("Window"), this);
    QAction* toggle_full = window_menu->addAction(tr("Toggle Fullscreen"));
    connect(toggle_full, &QAction::triggered,
            this, &ImageDisplayWidget::onToggleFullscreen);
    QMenu* tool_menu = menu_bar_->addMenu(tr("Tools"));
    QAction* toggle_side_bar = tool_menu->addAction(tr("Toggle Side Bar"));
    toggle_side_bar->setShortcut(QKeySequence("Alt+0"));
    connect(toggle_side_bar, &QAction::triggered,
            this, [=](){
        if(main_splitter_->sizes()[1] == 0)
            main_splitter_->setSizes(QList<int>() << 300 << 100);
        else
            main_splitter_->setSizes(QList<int>() << 100 << 0);
    });
}

void ImageDisplayWidget::initLayout()
{
    QVBoxLayout* layout = new QVBoxLayout;

    QGroupBox* token_box = new QGroupBox(tr("Tokens"), this);
    token_box->setLayout(new QHBoxLayout);
    token_box->layout()->addWidget(token_config_list_);

    QGroupBox* shape_box = new QGroupBox(tr("Shapes"), this);
    shape_box->setLayout(new QHBoxLayout);
    shape_box->layout()->addWidget(shape_config_list_);

    QVBoxLayout* config_layout = new QVBoxLayout;
    config_layout->addWidget(token_box, 1);
    config_layout->addWidget(shape_box, 1);
    config_layout->setContentsMargins(0,0,0,0);

    Container* container = new Container(this);
    container->setLayout(config_layout);

    //main_splitter_->setStyleSheet("Container { color: white; background-color: black; }");
    main_splitter_->addWidget(view_);
    main_splitter_->addWidget(container);
    main_splitter_->setSizes(QList<int>() << 100 << 0);

    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(menu_bar_, -1);
    layout->addWidget(main_splitter_, 1000000);

    setLayout(layout);
}
