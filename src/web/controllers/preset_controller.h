#ifndef WEB_PRESET_CONTROLLER_HANDLER_H
#define WEB_PRESET_CONTROLLER_HANDLER_H

#include "httprequesthandler.h"

#include <QMultiMap>

#include "tile/graphics_view.h"
#include "web/response.h"

namespace Web {

/**
 * @brief The PresetController class parses and handles incoming preset requests
 * forwarded by an instance of Web::RequestHandler (see request_handler.h)
 */
class PresetController : public stefanfrings::HttpRequestHandler
{
    Q_OBJECT
private:
    /**
     * @brief The FunctionIndex enum references function names
     * by int.
     */
    enum FunctionIndex {
        NONE,
        PLAY,
        STOP,
        IS_PLAYING,
        GET_VOLUME,
        SET_VOLUME,
        GET_STATE
    };

    /**
     * @brief converts given function index to string.
     * @param idx: function index to convert
     * @return converted string
     */
    static QString const toString(FunctionIndex idx);

    /**
     * @brief converts given function index to bytearray.
     * @param idx: function index to convert
     * @return converted string
     */
    static QByteArray const toByteArray(FunctionIndex idx);

    /**
     * @brief converts given string to function index.
     * @param str: string to convert
     * @return function index converted.
     */
    static FunctionIndex toIndex(const QString& str);

    /**
     * @brief converts given byte array to function index.
     * @param arr: byte array to convert
     * @return function index converted.
     */
    static FunctionIndex toIndex(const QByteArray& arr);

public:
    PresetController(QObject* parent=0);
    /**
     * @brief Evaluates forwarded requests and replies using response.
     * @param request: instance encoding the request forwarded.
     * @param response: instance the response is written to.
     */
    void service(stefanfrings::HttpRequest& request, stefanfrings::HttpResponse& response);

    /**
     * @brief sets the underlying preset view, so that behavior triggered by
     * this class can effect given preset view.
     * @param preset_view
     */
    void setPresetView(Tile::GraphicsView* preset_view);
private:
    /**
     * @brief writes project as json to response message.
     * @param response: instance the response is written to.
     */
    void respondProject(stefanfrings::HttpResponse& response);

    /**
     * @brief writes not implemented error to response.
     * @param response: instance the response is written to.
     */
    void respondNotImplemented(stefanfrings::HttpResponse& response);

    /**
     * @brief writes no preset view error to response.
     * @param response: instance the response is written to.
     */
    void respondNoPresetView(stefanfrings::HttpResponse& response);

    /**
     * @brief Parses parameter list to activate tile
     * identified by id in parameter list.
     * Writes success state of tile activation to response.
     * @param parameters: List of request key, value parameters
     * @param response: instance the response is written to.
     */
    void parsePlayRequest(const QMultiMap<QByteArray,QByteArray>& parameters,
                          stefanfrings::HttpResponse& response);

    /**
     * @brief Parses parameter list to deactivate tile
     * identified by id in parameter list.
     * Writes success state of tile deactivation to response.
     * @param parameters: List of request key, value parameters
     * @param response: instance the response is written to.
     */
    void parseStopRequest(const QMultiMap<QByteArray,QByteArray>& parameters,
                          stefanfrings::HttpResponse& response);

    /**
     * @brief Parses parameter list to check activation
     * of tile identified by id in parameter list.
     * Writes activation state of tile to response.
     * @param parameters: List of request key, value parameters
     * @param response: insatnce the response is written to.
     */
    void parseIsPlayingRequest(const QMultiMap<QByteArray,QByteArray>& parameters,
                          stefanfrings::HttpResponse& response);

    /**
     * @brief Parses parameter list to check volume
     * of tile identified by id in parameter list
     * @param parameters: List of request key, value parameters
     * @param response: instance the response is written to.
     */
    void parseGetVolumeRequest(const QMultiMap<QByteArray,QByteArray>& parameters,
                               stefanfrings::HttpResponse& response);

    /**
     * @brief Parses parameter list to set volume
     * of tile identified by id in parameter list
     * @param parameters: List of request key, value parameters
     * @param response: instance the response is written to.
     */
    void parseSetVolumeRequest(const QMultiMap<QByteArray,QByteArray>& parameters,
                               stefanfrings::HttpResponse& response);

    /**
     * @brief Parses parameter list to return all state info
     * for tile identified by uuid.
     * @param parameters: List of request, key value parameters.
     * @param response: instance the response is written to.
     */
    void parseGetState(const QMultiMap<QByteArray,QByteArray>& parameters,
                       stefanfrings::HttpResponse& response);

    /**
     * @brief writes given Web::Response message to given response instance.
     * Additionally produces log of given response.
     * @param response_msg: instance used to produce formatted response message.
     * @param response: instance to write response to.
     * @param reply_state: state for response.write(...) call.
     */
    void writeAndLog(const Response& response_msg,
                     stefanfrings::HttpResponse& response, bool reply_state);

    Tile::GraphicsView* preset_view_;
};

} // namespace Web

#endif // WEB_PRESET_CONTROLLER_HANDLER_H
