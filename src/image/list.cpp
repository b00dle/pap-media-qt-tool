#include "list.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QFileInfoList>
#include <QDir>
#include <QString>
#include <QHeaderView>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QCoreApplication>

#include "image_item.h"

namespace Image {

List::List(QWidget *parent)
    : QWidget(parent)
    , model_(0)
    , file_view_(0)
    , presentation_view_(0)
    , open_button_(0)
    , line_edit_(0)
{
    initWidgets();
    initLayout();
}

List::~List()
{
    if(presentation_view_)
        presentation_view_->deleteLater();
}

void List::openDirectory(const QString& dir_name)
{
    QDir dir(dir_name);
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.bmp";
    QFileInfoList image_files = dir.entryInfoList(filters, QDir::Files|QDir::NoDotAndDotDot);
    model_->removeRows(0, model_->rowCount());
    model_->setRowCount(image_files.size());
    int i = 0;
    foreach (QFileInfo info, image_files) {
        QString path = info.absoluteFilePath();
        QString name = info.fileName();
        model_->setData(model_->index(i,0), QVariant(name), Qt::DisplayRole);
        model_->setData(model_->index(i,0), QVariant(path), Qt::UserRole);
        model_->setData(model_->index(i,0), QVariant("<html><div><img src=\""+path+"\" style=\"width:100%;\" height=\"200\" /></div></html>"), Qt::ToolTipRole);
        ++i;
    }
    line_edit_->setText(dir_name);
}

View *List::getView() const
{
    return presentation_view_->getView();
}

ImageDisplayWidget *List::getDisplayWidget() const
{
    return presentation_view_;
}

void List::onOpen()
{
    QString dir_name = QFileDialog::getExistingDirectory(this, tr("Open Directory"));
    if(dir_name.size() == 0) {
        qDebug() << "nothing opened";
        return;
    }
    openDirectory(dir_name);
    emit newDirectory(dir_name);
}

void List::onImageSelected(const QModelIndex &idx)
{
    if(!idx.isValid())
        return;
    onImageSelected(idx.row());
}

void List::onImageSelected(int row)
{
    if(row < 0 || row >= model_->rowCount())
        return;
    QString path = model_->data(model_->index(row, 0), Qt::UserRole).toString();
    presentation_view_->getView()->setItem(new ImageItem(path));
    presentation_view_->popOpen();
}

void List::initWidgets()
{
    model_ = new QStandardItemModel(0, 1, this);
    model_->setHeaderData(0, Qt::Horizontal, QVariant("Name"));

    file_view_ = new QListView(this);
    file_view_->setModel(model_);
    file_view_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //file_view_->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(file_view_, SIGNAL(clicked(const QModelIndex&)),
            this, SLOT(onImageSelected(const QModelIndex&)));

    presentation_view_ = new ImageDisplayWidget;
    presentation_view_->setMinimumSize(640, 480);
    presentation_view_->setWindowFlags(Qt::Window);
    presentation_view_->setWindowTitle(tr("Companion Stage"));
    presentation_view_->hide();

    open_button_ = new QPushButton(tr("browse"), this);
    connect(open_button_, SIGNAL(clicked()),
            this, SLOT(onOpen()));

    line_edit_ = new QLineEdit(this);
    line_edit_->setReadOnly(true);
}

void List::initLayout()
{
    QVBoxLayout* layout = new QVBoxLayout;

    QHBoxLayout* lower_layout = new QHBoxLayout;
    lower_layout->addWidget(open_button_,0);
    lower_layout->addWidget(line_edit_,5);
    lower_layout->setContentsMargins(5, 0, 5, 5);

    layout->addWidget(file_view_, 5);
    layout->addLayout(lower_layout, 0);
    setLayout(layout);
}

} // namespace Image
