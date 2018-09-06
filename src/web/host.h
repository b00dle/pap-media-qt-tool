#ifndef WEB_HOST_H
#define WEB_HOST_H

#include <QDialog>
#include <QLineEdit>

#include "httplistener.h"
#include "request_handler.h"
#include "tile/graphics_view.h"
#include "app/chat.h"
#include "app/image_loader.h"
#include "log/file_logger.h"

namespace Web {

class Host : public QDialog
{
    Q_OBJECT
public:
    explicit Host(QWidget *parent = 0);
    ~Host();

    QString const& getAddress() const;
    void setPresetView(Tile::GraphicsView* preset_view);
signals:

public slots:

private:
    void initListener();
    void initWidgets();
    void initLayout();
    const QString generateWebHostLinks();

    // Start the HTTP server
    stefanfrings::HttpListener* listener_;
    RequestHandler* request_handler_;
    QString address_;

    QLineEdit* line_edit_;
    App::Chat* chat_app_;
    App::ImageLoader* image_app_;
    Log::FileLogger logger_;
};

} // namespace Web
#endif // HOST_H
