#include "image_directory_model.h"
#include <QDateTime>
#include <QtConcurrent>

#include <QPainter>

ImageDirectoryModel::ImageDirectoryModel(QObject* parent)
    : QFileSystemModel(parent)
    , mutex_()
    , thumbnails_()
    , pixmap_cache_()
    , thumbnail_size_(200)
    , display_mode_(ThumbnailMode::Full)
    , show_filenames(true)

{
    initialize();
}

ImageDirectoryModel::ImageDirectoryModel(int thumbnail_size, QObject* parent)
    : QFileSystemModel(parent)
    , mutex_()
    , thumbnails_()
    , pixmap_cache_()
    , thumbnail_size_(thumbnail_size)
    , display_mode_(ThumbnailMode::Full)
    , show_filenames(true)
{
    initialize();
}

ImageDirectoryModel::ImageDirectoryModel(int thumbnail_size, ThumbnailMode mode, QObject* parent)
    : QFileSystemModel(parent)
    , mutex_()
    , thumbnails_()
    , pixmap_cache_()
    , thumbnail_size_(thumbnail_size)
    , display_mode_(mode)
    , show_filenames(true)
{
    initialize();
}

ImageDirectoryModel::~ImageDirectoryModel()
{
    QMutexLocker lock(&mutex_);
    thumbnails_.clear();
    pixmap_cache_.clear();
}

QVariant ImageDirectoryModel::data(const QModelIndex& index, int role) const
{
    QFileInfo info = fileInfo(index);
    if (role == Qt::DecorationRole) {

        if (info.isFile() && nameFilters().contains("*." + info.suffix())) {

            QPixmap pixmap;
            bool found = false;
            bool in_cache = false;
            QMutexLocker lock(&mutex_);
            auto it = thumbnails_.find(info.filePath());
            if (it != thumbnails_.end()) {
                if (it.value().isValid()) {
                    if (it.value() <= info.lastModified()) {
                        in_cache = pixmap_cache_.find(info.filePath(), &pixmap);
                        found = true;
                    } else {
                        found = false;
                    }
                }
            } else {
                thumbnails_.insert(info.filePath(), info.lastModified());
                found = false;
            }

            if (!found) {
                QtConcurrent::run(const_cast<ImageDirectoryModel *>(this),
                                  &ImageDirectoryModel::loadThumbnail,
                                  info.filePath(),
                                  info.lastModified());
                return QColor(Qt::red);
            } else {
                if(in_cache)
                    return QIcon(pixmap);
                else
                    return QColor(Qt::red);
            }
        }

    } else if (role == Qt::ToolTipRole) {
        return QVariant("<html><head></head><body><div>"+info.fileName()+"</div></body></html>");
    } else if (role == Qt::UserRole) {
        return QVariant(info.filePath());
    } else if (role == Qt::DisplayRole) {

        if(show_filenames)
            return QVariant(info.fileName());
        else
            return QVariant();
    }

    return QVariant();
}

void ImageDirectoryModel::toggleFileNames()
{
    show_filenames = !show_filenames;
}


void ImageDirectoryModel::loadThumbnail(const QString& path, const QDateTime &time)
{

    QByteArray image_data;
    bool loaded = false;
    if(!loadImageFromFile(path, image_data))
    {
        qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
        qDebug() << "  >" << "Error: loading the image from file failed!";
    } else {
        loaded = true;
    }

    QPixmap pixmap;
    if(!pixmap.loadFromData(image_data, image_data.data()))
        return;

    if(pixmap.isNull())
        qDebug() << "Error: image" << path << "is null";

    bool changed = false;
    QMutexLocker lock(&mutex_);
    auto it = thumbnails_.find(path);
    if (it != thumbnails_.end() && it.value() <= time)
    {
        if (loaded) {
            *it = time;

            QPixmap new_image;
            if(!pixmap.isNull()) {
                createSquareThumbnail(pixmap);
                if(!pixmap_cache_.insert(path, pixmap)) {
                    qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
                    qDebug() << "  >" << "Error: inserting image with path:" << path;
                } else {
                    qDebug() << "  >" << "Added image" << path;
                }
            }

        } else {
            thumbnails_.erase(it);
            pixmap_cache_.remove(path);
        }
        changed = true;
    }

    if (changed)
        emit thumbnailLoaded(path);
}

void ImageDirectoryModel::createSquareThumbnail(QPixmap &pixmap) const
{
    // add space to which ever side is the smaller one
    if(display_mode_ == ThumbnailMode::Full){

        auto scaled = pixmap.scaled(thumbnail_size_, thumbnail_size_, Qt::KeepAspectRatio);

        QPoint starting_point;
        if(pixmap.width() > pixmap.height()) {
            int tb_spacing = (thumbnail_size_ - scaled.height()) / 2;
            starting_point = QPoint(0, tb_spacing);
        } else if (pixmap.width() < pixmap.height()) {
            int lr_spacing = (thumbnail_size_ - scaled.width()) / 2;
            starting_point = QPoint(lr_spacing, 0);
        } else {
            pixmap = scaled;
            return;
        }

        QImage new_img(thumbnail_size_, thumbnail_size_, QImage::Format_ARGB32);
        new_img.fill(Qt::transparent);

        QPainter painter(&new_img);
        QImage original_image = scaled.toImage();
        painter.drawImage(starting_point, original_image);
        painter.end();

        pixmap = QPixmap::fromImage(new_img);

    } else { // select a centered cutout of the image

        QRect cutout;
        QPixmap scaled;
        if(pixmap.width() > pixmap.height()) {
            scaled = pixmap.scaledToHeight(thumbnail_size_);
            int overlap = (scaled.width() - thumbnail_size_) / 2;
            cutout = QRect(overlap, 0, thumbnail_size_, thumbnail_size_);
        } else if( pixmap.width() < pixmap.height()) {
            scaled = pixmap.scaledToWidth(thumbnail_size_);
            int overlap = (scaled.height() - thumbnail_size_) / 2;
            cutout = QRect(0, overlap, thumbnail_size_, thumbnail_size_);

        } else {
            pixmap = pixmap.scaled(thumbnail_size_, thumbnail_size_);
            return;
        }

        pixmap = scaled.copy(cutout);
    }
}

void ImageDirectoryModel::initialize()
{
    pixmap_cache_.setCacheLimit(100000);
    connect(this, &ImageDirectoryModel::thumbnailLoaded,
            this, &ImageDirectoryModel::updateThumbnail , Qt::QueuedConnection);

    connect(this, &QFileSystemModel::rootPathChanged,
            this, &ImageDirectoryModel::onRootPathChanged);
}

bool ImageDirectoryModel::loadImageFromFile(const QString &filepath, QByteArray &to_fill) const
{
    QFile file(filepath);
    if(!file.open(QIODevice::ReadOnly))
        return false;
    to_fill = file.readAll();

    return true;
}

void ImageDirectoryModel::updateThumbnail(const QString &path)
{
    QModelIndex i = index(path);
    if (i.isValid())
        emit dataChanged(i, i, QVector<int>{QFileSystemModel::FileIconRole});
}

void ImageDirectoryModel::onRootPathChanged(const QString &)
{
    pixmap_cache_.clear();
    thumbnails_.clear();
}
