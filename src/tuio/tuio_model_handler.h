#ifndef TUIO_MODEL_HANDLER_H
#define TUIO_MODEL_HANDLER_H

#include "qtuiohandler.h"
#include "models/tuio_cursor_table_model.h"
#include "models/tuio_token_table_model.h"
#include "models/tuio_blob_table_model.h"

class TuioModelHandler : public QTuioHandler
{
    Q_OBJECT
public:
    explicit TuioModelHandler(QObject *parent = nullptr);
    explicit TuioModelHandler(const QHostAddress& ip, unsigned port, QObject *parent = nullptr);
    virtual ~TuioModelHandler();

    TuioCursorTableModel* getCursorModel() const;
    TuioTokenTableModel* getTokenModel() const;
    TuioBlobTableModel* getBlobModel() const;

signals:

public slots:

protected:
    void initModels();

    TuioCursorTableModel* cursor_model_;
    TuioTokenTableModel* token_model_;
    TuioBlobTableModel* blob_model_;
};

#endif // TUIO_MODEL_HANDLER_H
