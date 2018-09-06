#include "preset_controller.h"

#include <QDebug>
#include <QList>
#include <QJsonDocument>
#include <QJsonArray>
#include <QUuid>

namespace Web {

const QString PresetController::toString(PresetController::FunctionIndex idx)
{
    QString str = "";
    switch(idx) {
        case PLAY:
            str = "play"; break;
        case STOP:
            str = "stop"; break;
        case IS_PLAYING:
            str = "is_playing"; break;
        case GET_VOLUME:
            str = "get_volume"; break;
        case SET_VOLUME:
            str = "set_volume"; break;
        case GET_STATE:
            str = "get_state"; break;
        default:
            break;
    }
    return str;
}

const QByteArray PresetController::toByteArray(PresetController::FunctionIndex idx)
{
    return QByteArray(toString(idx).toStdString().c_str());
}

PresetController::FunctionIndex PresetController::toIndex(const QString &str)
{
    if(str.compare("play") == 0)
        return PLAY;
    else if(str.compare("stop") == 0)
        return STOP;
    else if(str.compare("is_playing") == 0)
        return IS_PLAYING;
    else if(str.compare("get_volume") == 0)
        return GET_VOLUME;
    else if(str.compare("set_volume") == 0)
        return SET_VOLUME;
    else if(str.compare("get_state") == 0)
        return GET_STATE;
    else
        return NONE;
}

PresetController::FunctionIndex PresetController::toIndex(const QByteArray &arr)
{
    return toIndex(QString(arr));
}

PresetController::PresetController(QObject* parent)
    : stefanfrings::HttpRequestHandler(parent)
    , preset_view_(0)
{}

void PresetController::service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response)
{
    if(preset_view_ == 0)
        respondNoPresetView(response);

    QMultiMap<QByteArray,QByteArray> parameters = request.getParameterMap();

    if(parameters.size() == 0) {
        respondProject(response);
    } else if(parameters.contains(toByteArray(PLAY))) {
        parsePlayRequest(parameters, response);
    } else if(parameters.contains(toByteArray(STOP))) {
        parseStopRequest(parameters, response);
    } else if(parameters.contains(toByteArray(IS_PLAYING))) {
        parseIsPlayingRequest(parameters, response);
    } else if(parameters.contains(toByteArray(SET_VOLUME))) {
        parseSetVolumeRequest(parameters, response);
    } else if(parameters.contains(toByteArray(GET_VOLUME))) {
        parseGetVolumeRequest(parameters, response);
    } else if(parameters.contains(toByteArray(GET_STATE))) {
        parseGetState(parameters, response);
    } else {
        respondNotImplemented(response);
    }
}

void PresetController::setPresetView(Tile::GraphicsView *preset_view)
{
    preset_view_ = preset_view;
}

void PresetController::respondProject(stefanfrings::HttpResponse &response)
{
    Response r("", QJsonValue(preset_view_->toJsonObject()), "project");
    writeAndLog(r, response, true);
}

void PresetController::respondNotImplemented(stefanfrings::HttpResponse &response)
{
    Response r("NOT IMPLEMENTED (PRESET CONTROLLER)", QJsonValue(""), "");
    writeAndLog(r, response, true);
}

void PresetController::respondNoPresetView(stefanfrings::HttpResponse &response)
{
    Response r("NO PRESET VIEW SET ON SERVER", QJsonValue(""), "");
    writeAndLog(r, response, true);
}

void PresetController::parsePlayRequest(const QMultiMap<QByteArray, QByteArray> &parameters, stefanfrings::HttpResponse &response)
{
    QByteArray play_what = parameters.value(toByteArray(PLAY));
    QUuid tile_uuid = QUuid(QString(play_what));
    QJsonObject obj;
    obj["id"] = tile_uuid.toString();
    obj["success"] = preset_view_->activate(tile_uuid);
    Response r("", QJsonValue(obj), toString(PLAY));
    writeAndLog(r, response, true);
}

void PresetController::parseStopRequest(const QMultiMap<QByteArray, QByteArray> &parameters, stefanfrings::HttpResponse &response)
{
    QByteArray stop_what = parameters.value(toByteArray(STOP));
    QUuid tile_uuid = QUuid(QString(stop_what));
    QJsonObject obj;
    obj["id"] = tile_uuid.toString();
    obj["success"] = preset_view_->deactivate(tile_uuid);
    Response r("", QJsonValue(obj), toString(STOP));
    writeAndLog(r, response, true);
}

void PresetController::parseIsPlayingRequest(const QMultiMap<QByteArray, QByteArray> &parameters, stefanfrings::HttpResponse &response)
{
    QByteArray is_playing_what = parameters.value(toByteArray(IS_PLAYING));
    QUuid tile_uuid = QUuid(QString(is_playing_what));
    QJsonObject obj;
    obj["id"] = QString(is_playing_what);
    obj["is_playing"] = preset_view_->isActivated(tile_uuid);
    Response r("", QJsonValue(obj), toString(IS_PLAYING));
    writeAndLog(r, response, true);
}

void PresetController::parseGetVolumeRequest(const QMultiMap<QByteArray, QByteArray> &parameters, stefanfrings::HttpResponse &response)
{
    QByteArray get_column_what = parameters.value(toByteArray(GET_VOLUME));
    QUuid tile_uuid = QUuid(QString(get_column_what));
    QJsonObject obj;
    obj["id"] = QString(get_column_what);
    obj["volume"] = preset_view_->getVolume(tile_uuid);
    Response r("", QJsonValue(obj), toString(GET_VOLUME));
    writeAndLog(r, response, true);
}

void PresetController::parseSetVolumeRequest(const QMultiMap<QByteArray, QByteArray> &parameters, stefanfrings::HttpResponse &response)
{
    QByteArray get_column_what = parameters.value(toByteArray(SET_VOLUME));
    QByteArray volume_value = parameters.value("value");
    QUuid tile_uuid = QUuid(QString(get_column_what));
    QJsonObject obj;
    obj["id"] = tile_uuid.toString();
    obj["success"] = preset_view_->setVolume(tile_uuid, volume_value.toInt());
    Response r("", QJsonValue(obj), toString(SET_VOLUME));
    writeAndLog(r, response, true);
}

void PresetController::parseGetState(const QMultiMap<QByteArray, QByteArray> &parameters, stefanfrings::HttpResponse &response)
{
    QByteArray get_column_what = parameters.value(toByteArray(GET_STATE));
    QUuid tile_uuid = QUuid(QString(get_column_what));
    QJsonObject obj;
    obj["id"] = QString(get_column_what);
    obj["volume"] = preset_view_->getVolume(tile_uuid);
    obj["is_playing"] = preset_view_->isActivated(tile_uuid);
    Response r("", QJsonValue(obj), toString(GET_STATE));
    writeAndLog(r, response, true);
}

void PresetController::writeAndLog(const Response &response_msg, stefanfrings::HttpResponse &response, bool reply_state)
{
    QByteArray reply = response_msg.toByteArray();
    /*qDebug() << "=========================";
    qDebug() << "RESPONSE SENT.";
    qDebug() << " > " << reply;*/
    response.setHeader("Access-Control-Allow-Origin", "*");
    response.write(reply, reply_state);
}

} // namespace Web
