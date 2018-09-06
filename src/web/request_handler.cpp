#include "request_handler.h"

#include <QDebug>
#include <QByteArray>

#include "resources/lib.h"

namespace Web {

RequestHandler::RequestHandler(QObject* parent)
    : stefanfrings::HttpRequestHandler(parent)
    , preset_controller_(0)
    , chat_controller_(0)
    , image_controller_(0)
{
    preset_controller_ = new PresetController(this);
    chat_controller_ = new ChatController(this);
    image_controller_ = new ImageController(this);
}

void RequestHandler::service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response)
{
    QString path(request.getPath());
    if (path.compare("/preset") == 0) {
        preset_controller_->service(request, response);
    } else if(path.compare("/chat") == 0) {
        chat_controller_->service(request, response);
    } else if(path.compare("/image") == 0) {
        image_controller_->service(request, response);
    } else {
        response.write("NOT IMPLEMENTED",true);
    }
}

void RequestHandler::setPresetView(Tile::GraphicsView *preset_view)
{
    preset_controller_->setPresetView(preset_view);
}

App::ChatMessageModel *RequestHandler::getChatMessageModel()
{
    return chat_controller_->getModel();
}

void RequestHandler::onNewImageLoaded(const QString& str)
{
    image_controller_->onNewImageLoaded(str);
}

} // namespace Web
