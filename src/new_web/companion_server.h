#ifndef COMPANION_SERVER_H
#define COMPANION_SERVER_H

#include "server.h"
#include "tile/graphics_view.h"

class CompanionServer : public Server {
    Q_OBJECT
public:
    explicit CompanionServer(int port = 41736, QObject *parent = nullptr);
    virtual ~CompanionServer();

    void setGraphicsView(Tile::GraphicsView* view);

signals:
    void messageSent(const QByteArray&);

protected slots:
    virtual void newConnection();

protected:
    virtual void sendToClient(const QByteArray& data, QTcpSocket* client);
    virtual void processClientMessage(const NetworkMessage &msg, QTcpSocket *client);

    Tile::GraphicsView* view_;
};

#endif // COMPANION_SERVER_H
