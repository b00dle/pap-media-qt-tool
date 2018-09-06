#ifndef SOCKET_HOST_WIDGET_H
#define SOCKET_HOST_WIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>

#include "companion_server.h"
#include "companion_udp_discovery.h"

class SocketHostWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SocketHostWidget(Tile::GraphicsView* view, QWidget *parent = nullptr);

signals:

public slots:

private slots:
    void onNameSubmit();

private:
    void initWidgets();
    void initLayout();

    CompanionServer* server_;
    CompanionUdpDiscovery* udp_discovery_;
    QTextEdit* text_edit_;
    QLineEdit* name_edit_;
    QPushButton* name_submit_;
};

#endif // SOCKET_HOST_WIDGET_H
