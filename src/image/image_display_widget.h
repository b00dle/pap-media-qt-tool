#ifndef IMAGE_DISPLAY_WIDGET_H
#define IMAGE_DISPLAY_WIDGET_H

#include <QWidget>
#include <QMenuBar>
#include <QSplitter>

class InteractiveImageToken;
class InteractiveImageShape;

namespace Image {
class View;
}
class WidgetListView;

class ImageDisplayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageDisplayWidget(QWidget *parent = nullptr);

    Image::View* getView() const;
    void popOpen();

private slots:
    void onItemSet();
    void onToggleFullscreen();
    void onZoomIn();
    void onZoomOut();
    void onTokenAdded(InteractiveImageToken*);
    void onShapeAdded(InteractiveImageShape*);
    void removeAllTokenConfigs();
    void removeAllShapeConfigs();

private:
    void keyPressEvent(QKeyEvent *event);

    void initWidgets();
    void initMenu();
    void initLayout();

    QMenuBar* menu_bar_;
    QMenu* view_menu_;
    Image::View* view_;
    WidgetListView* token_config_list_;
    WidgetListView* shape_config_list_;
    QSplitter* main_splitter_;
};

#endif // IMAGE_DISPLAY_WIDGET_H
