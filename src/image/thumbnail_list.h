#ifndef IMAGELIST_H
#define IMAGELIST_H

#include <QObject>
#include <QWidget>

#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QSlider>
#include <QLabel>

#include "models/image_directory_model.h"
#include "image_display_widget.h"
#include "view.h"

class ThumbnailList : public QWidget
{
        Q_OBJECT
    public:

        enum ViewMode {List, Grid};

        /**
         * @brief ImageList c'tor
         * @param parent
         */
        explicit ThumbnailList(QWidget *parent = nullptr);
        virtual ~ThumbnailList() override;

        /**
         * @brief Overridden resizeEvent
         * @param event
         */
        virtual void resizeEvent(QResizeEvent *event) override;

        void openDirectory(const QString&);
        Image::View *getView() const;
        ImageDisplayWidget *getDisplayWidget() const;

    signals:
        void newDirectory(const QString& new_directory);

    public slots:
        void onOpen();

        void onImageSelected(const QModelIndex& idx);


    private slots:
        void onChangeGridsize(int value);

    private:
        void initWidgets();
        void initLayout();

        int gridsize_;
        ViewMode view_mode_;

        ImageDirectoryModel *model_;

        QListView *file_view_;
        QLineEdit *line_edit_;
        QPushButton *open_button_;
        QSlider *gridsize_slider_;
        QLabel *list_icon_label_;
        QLabel *grid_icon_label_;

        ImageDisplayWidget *presentation_view_;
};

#endif // IMAGELIST_H
