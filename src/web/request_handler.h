#ifndef WEB_REQUEST_HANDLER_H
#define WEB_REQUEST_HANDLER_H

#include "httprequesthandler.h"
#include "controllers/preset_controller.h"
#include "controllers/chat_controller.h"
#include "controllers/image_controller.h"
#include "app/chat_message_model.h"
#include "tile/graphics_view.h"

namespace Web {

class RequestHandler : public stefanfrings::HttpRequestHandler {
    Q_OBJECT
public:
    RequestHandler(QObject* parent=0);
    void service(stefanfrings::HttpRequest& request, stefanfrings::HttpResponse& response);
    void setPresetView(Tile::GraphicsView* preset_view);

    App::ChatMessageModel* getChatMessageModel();

public slots:
    void onNewImageLoaded(QString const& str);
private:
    PresetController* preset_controller_;
    ChatController* chat_controller_;
    ImageController* image_controller_;
};

} // namespace Web

#endif // WEB_REQUEST_HANDLER_H
