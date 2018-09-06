#include "tuio_model_handler.h"

TuioModelHandler::TuioModelHandler(QObject *parent)
    : QTuioHandler(parent)
    , cursor_model_(0)
    , token_model_(0)
{
    initModels();
}

TuioModelHandler::TuioModelHandler(const QHostAddress &ip, unsigned port, QObject *parent)
    : QTuioHandler(ip, port, parent)
    , cursor_model_(0)
    , token_model_(0)
{
    initModels();
}

TuioModelHandler::~TuioModelHandler()
{}

TuioCursorTableModel *TuioModelHandler::getCursorModel() const
{
    return cursor_model_;
}

TuioTokenTableModel *TuioModelHandler::getTokenModel() const
{
    return token_model_;
}

TuioBlobTableModel *TuioModelHandler::getBlobModel() const
{
    return blob_model_;
}

void TuioModelHandler::initModels()
{
    token_model_ = new TuioTokenTableModel(this);
    connect(this, &TuioModelHandler::tokenEvent,
            token_model_, &TuioTokenTableModel::onTokenEvent);

    cursor_model_ = new TuioCursorTableModel(this);
    connect(this, &TuioModelHandler::cursorEvent,
            cursor_model_, &TuioCursorTableModel::onCursorEvent);

    blob_model_ = new TuioBlobTableModel(this);
    connect(this, &TuioModelHandler::blobEvent,
            blob_model_, &TuioBlobTableModel::onBlobEvent);
}
