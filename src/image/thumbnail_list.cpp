#include "thumbnail_list.h"

#include <QDebug>
#include <QVBoxLayout>

#include <QFileSystemModel>
#include <QDir>
#include <QResizeEvent>
#include <QPixmap>
#include <QFileDialog>

#include "image_item.h"

ThumbnailList::ThumbnailList(QWidget *parent)
    : QWidget(parent)
    , gridsize_(1)
    , view_mode_(ViewMode::List)
    , model_(Q_NULLPTR)
    , file_view_(Q_NULLPTR)
    , line_edit_(Q_NULLPTR)
    , open_button_(Q_NULLPTR)
    , gridsize_slider_(Q_NULLPTR)
    , list_icon_label_(Q_NULLPTR)
    , grid_icon_label_(Q_NULLPTR)
{
    initWidgets();
    initLayout();
}

ThumbnailList::~ThumbnailList()
{}

void ThumbnailList::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    QSize file_view_size(file_view_->size().width(), file_view_->size().height());
    if(gridsize_ == 1) {

        if(view_mode_ != ViewMode::List) {
            view_mode_ = ViewMode::List;
            file_view_->setViewMode(QListView::ListMode);
            model_->toggleFileNames();
        }

    } else {

        if(view_mode_!= ViewMode::Grid) {
            view_mode_ = ViewMode::Grid;
            file_view_->setViewMode(QListView::IconMode);
            model_->toggleFileNames();
        }

        QSize icon_size((file_view_size.width() / gridsize_) - 10, (file_view_size.width() / gridsize_) -10);
        file_view_->setIconSize(icon_size);
    }

}

void ThumbnailList::openDirectory(const QString &dir_name)
{
    QDir dir(dir_name);
    if(!model_->index(dir_name).isValid())
        model_->setRootPath(dir.path());

    file_view_->setRootIndex(model_->index(dir.path()));
}

Image::View *ThumbnailList::getView() const
{
    return presentation_view_->getView();
}

ImageDisplayWidget *ThumbnailList::getDisplayWidget() const
{
    return presentation_view_;
}

void ThumbnailList::onOpen()
{
    QString dir_name = QFileDialog::getExistingDirectory(this, tr("Open Directory"));
    if(dir_name.size() == 0) {
        qDebug() << "nothing opened";
        return;
    }
    openDirectory(dir_name);
    emit newDirectory(dir_name);
}

void ThumbnailList::onImageSelected(const QModelIndex &idx)
{
    if(!idx.isValid())
        return;

    QString path = model_->data(idx, Qt::UserRole).toString();

    qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
    qDebug() << "  >" << path;

    presentation_view_->getView()->setItem(new ImageItem(path));
    presentation_view_->popOpen();
}

void ThumbnailList::onChangeGridsize(int value)
{
    gridsize_ = value;
    if (gridsize_ == 1) {
        if(view_mode_ != ViewMode::List) {
            view_mode_ = ViewMode::List;
            file_view_->setViewMode(QListView::ListMode);
            model_->toggleFileNames();
        }

        QSize icon_size;
        if(file_view_->size().width() > 200)
            icon_size = QSize(100, 100);
        else
            icon_size = QSize(file_view_->size().width(), file_view_->size().width());

        file_view_->setIconSize(icon_size);

    } else {
        if(view_mode_!= ViewMode::Grid) {
            view_mode_ = ViewMode::Grid;
            file_view_->setViewMode(QListView::IconMode);
            model_->toggleFileNames();
        }

        QSize icon_size(file_view_->size().width() / gridsize_, file_view_->size().width() / gridsize_);
        file_view_->setIconSize(icon_size);
    }
}

void ThumbnailList::initWidgets()
{
    model_ = new ImageDirectoryModel(200, ImageDirectoryModel::ThumbnailMode::Cutout ,this);

    QDir user_home = QDir::homePath();
    model_->setRootPath(user_home.path());
    model_->setReadOnly(false);
    model_->setFilter(QDir::Files | QDir::Dirs | QDir::NoDot);
    model_->setNameFilters(QStringList{ "*.png", "*.jpg", "*.JPG", "*.jpeg", "*.bmp" });
    model_->setNameFilterDisables(false);

    file_view_ = new QListView(this);
    file_view_->setModel(model_);
    file_view_->setMovement(QListView::Static);
    file_view_->setViewMode(QListView::ListMode);
    file_view_->setResizeMode(QListView::Adjust);
    file_view_->setIconSize(QSize(100, 100));
    file_view_->setEditTriggers(QListView::NoEditTriggers);
    connect(file_view_, &QListView::clicked,
            this, &ThumbnailList::onImageSelected);

    list_icon_label_ = new QLabel(this);
    list_icon_label_->setPixmap(QPixmap(":/icons/ic_view_list_24px.svg"));

    grid_icon_label_ = new QLabel(this);
    grid_icon_label_->setPixmap(QPixmap(":/icons/ic_view_module_24px.svg"));

    gridsize_slider_ = new QSlider(Qt::Horizontal, this);
    gridsize_slider_->setMinimum(1);
    gridsize_slider_->setMaximum(4);
    gridsize_slider_->setTickInterval(1);
    gridsize_slider_->setValue(gridsize_);
    connect(gridsize_slider_, &QSlider::valueChanged,
            this, &ThumbnailList::onChangeGridsize);

    line_edit_ = new QLineEdit(this);
    line_edit_->setReadOnly(true);

    open_button_ = new QPushButton(tr("Browse"), this);
    connect(open_button_, &QPushButton::pressed,
            this, &ThumbnailList::onOpen);

    presentation_view_ = new ImageDisplayWidget;
    presentation_view_->setMinimumSize(640, 480);
    presentation_view_->setWindowFlags(Qt::Window);
    presentation_view_->setWindowTitle(tr("Compaion Stage"));
    presentation_view_->hide();

}

void ThumbnailList::initLayout()
{
    QHBoxLayout *resize_layout = new QHBoxLayout;
    resize_layout->addWidget(list_icon_label_);
    resize_layout->addWidget(gridsize_slider_);
    resize_layout->addWidget(grid_icon_label_);

    QHBoxLayout *browse_layout = new QHBoxLayout;
    browse_layout->addWidget(line_edit_);
    browse_layout->addWidget(open_button_);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(file_view_);
    layout->addLayout(resize_layout);
    layout->addLayout(browse_layout);

    setLayout(layout);
}
