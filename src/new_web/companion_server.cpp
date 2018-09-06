#include "companion_server.h"
#include "network_message.h"

#include <QMessageBox>

CompanionServer::CompanionServer(int port, QObject *parent)
    : Server(port, parent)
    , view_(0)
{}

CompanionServer::~CompanionServer()
{
    foreach(auto c, clients_)
        c->close();
}

void CompanionServer::setGraphicsView(Tile::GraphicsView *view)
{
    view_ = view;
}

void CompanionServer::newConnection()
{
    qDebug().nospace() << Q_FUNC_INFO << " @ line " << __LINE__;
    qDebug() << "  > " << "new incoming connection";
    // need to grab the socket
    QTcpSocket *socket = tcp_server_->nextPendingConnection();

    if(socket->bytesAvailable())
        qDebug() << "    > "  << "Client says:" << socket->readAll();

    if(view_) {
        NetworkMessage msg("set_project", view_->toJsonObject());
        sendToClient(msg.toByteArray(), socket);
    }
    clients_.push_back(socket);

    connect(socket, &QTcpSocket::disconnected,
            [=]()
        {
            qDebug() << "client disconnected";
            socket->close();
            clients_.removeAll(socket);
        }
    );
}

void CompanionServer::sendToClient(const QByteArray &data, QTcpSocket *client)
{
    Server::sendToClient(data, client);
    emit messageSent(data);
}

void CompanionServer::processClientMessage(const NetworkMessage &msg, QTcpSocket *client)
{
    qDebug().nospace() << Q_FUNC_INFO << " @ line " << __LINE__;
    qDebug() << "  > " << msg.toByteArray();
    if(msg.getMessage().compare("get_project") == 0) {
        if(view_) {
            NetworkMessage msg_reply("set_project", view_->toJsonObject());
            sendToClient(msg_reply.toByteArray(), client);
        }
    }
    else if(msg.getMessage().compare("toggle_tile_activation") == 0) {
        if(view_ && msg.getKwargs().contains("uuid")) {
            QUuid tile_uuid(msg.getKwargs()["uuid"].toString());
            Tile::BaseTile* t = view_->getTile(tile_uuid);
            if(!t)
                return;
            t->onActivate();
            QJsonObject kwargs;
            kwargs["uuid"] = QString(tile_uuid.toByteArray());
            kwargs["state"] = t->isActive();
            NetworkMessage msg_reply("set_tile_active", kwargs);
            sendToClient(msg_reply.toByteArray(), client);
        }
    }
    else if(msg.getMessage().compare("store_layout") == 0) {
        QString name = msg.getKwargs()["name"].toString();
        QJsonObject layout = msg.getKwargs()["json"].toObject();
        QString err = "";
        if(!layout.isEmpty()) {
            view_->storeAsLayout(name, layout);
        }
        else {
            err = "couldn't parse layout definition";
        }
        QJsonObject kwargs;
        kwargs["name"] = name;
        NetworkMessage msg_reply("store_layout", kwargs, err);
        sendToClient(msg_reply.toByteArray(), client);
    }
}
