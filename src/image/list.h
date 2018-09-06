#ifndef IMAGE_LIST_H
#define IMAGE_LIST_H

#include "view.h"

#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QStandardItemModel>
#include <QWidget>

#include "image_display_widget.h"
#include "view.h"

namespace Image {

class List : public QWidget
{
    Q_OBJECT
public:
    explicit List(QWidget *parent = 0);
    virtual ~List();

    void openDirectory(const QString&);
    View *getView() const;
    ImageDisplayWidget* getDisplayWidget() const;


signals:
    void newDirectory(const QString&);

public slots:

private slots:
    /* Opens a file open dialog to choose path for loading images. */
    void onOpen();

    /* Triggers opening of an image view, for respective image user has clicked. */
    void onImageSelected(const QModelIndex&);

    /* Triggers opening of an image view, for respective image user has clicked. */
    void onImageSelected(int row);

private:
    void initWidgets();
    void initLayout();

    QStandardItemModel* model_;
    QListView* file_view_;
    ImageDisplayWidget* presentation_view_;
    QPushButton* open_button_;
    QLineEdit* line_edit_;
};

} // namespace Image

#endif // IMAGE_LIST_H
