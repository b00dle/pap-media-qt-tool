#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H

#include <QObject>
#include <QFileSystemModel>

#include <QHash>
#include <QMutex>
#include <QPixmapCache>
#include <QFuture>
#include <QVector>

class ImageDirectoryModel: public QFileSystemModel
{
    Q_OBJECT

public:

    enum ThumbnailMode{Cutout, Full};

    /**
     * @brief ImageDirectoryModel default c'tor
     * @param parent
     */
    ImageDirectoryModel(QObject* parent = Q_NULLPTR);

    /**
     * @brief ImageDirectoryModel c'tor
     * @param thumbail_size Specifies the storage size of the final thumbnails
     * @param parent
     */
    ImageDirectoryModel(int thumbail_size, QObject* parent = Q_NULLPTR);

    /**
     * @brief ImageDirectoryModel c'tor
     * @param thumbail_size Specifies the storage size of the final thumbnails
     * @param mode Specifies whether a cutout of the image is shown
     *             or a downscaled version of the whole image
     * @param parent
     */
    ImageDirectoryModel(int thumbail_size, ThumbnailMode mode, QObject* parent = Q_NULLPTR);

    /**
     * @brief ~ImageDirectoryModel d'tor
     */
    virtual ~ImageDirectoryModel() override;

    QVariant data(const QModelIndex& index, int role) const override;

    /**
     * @brief Toggles returning the filenam as Qt::UserRole
     */
    void toggleFileNames();

    bool isImageAvailable(const QModelIndex& index) const;

signals:
    void thumbnailLoaded(const QString &);

protected slots:
    /**
     * @brief Emits dataChanged signal
     */
    void updateThumbnail(const QString &);

    /**
     * @brief Triggers when the rootIndex is changed
     */
    void onRootPathChanged(const QString &);

    /**
     * @brief Triggers when a new directory was loaded
     */
    void onDirectoryLoaded(const QString &);

protected:
    /**
     * @brief Loads thumbnail from given path
     *        Checks whether changes were made to each file
     * @param path
     * @param time
     */
    void loadThumbnail(const QString & path, const QDateTime & time);

    /**
     * @brief calculates square thumbnail from loaded pixmap
     * @param pixmap
     * @return
     */
    void createSquareThumbnail(QPixmap& pixmap) const;

    // members
    mutable QMutex mutex_;
    mutable QHash<QString, QDateTime> thumbnails_;
    mutable QPixmapCache pixmap_cache_;

    // vector containing all active created threads
    mutable QVector<QFuture<void>> active_threads_;

private:
    /**
     * @brief Init function
     */
    void initialize();

    /**
     * @brief Loads image from file to QByteArray
     * @param filename filepath
     * @param to_fill reference to container to be filled with the raw image data
     * @return
     */
    bool loadImageFromFile(const QString &filepath, QByteArray &to_fill) const;

    int thumbnail_size_;
    ThumbnailMode display_mode_;
    bool show_filenames;
};

#endif // IMAGEMODEL_H
