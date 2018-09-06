#ifndef COMPANION_UDP_DISCOVERY_H
#define COMPANION_UDP_DISCOVERY_H

#include <QObject>

#include "udp_broadcast_server.h"

class CompanionUdpDiscovery : public UdpBroadcastServer
{
    Q_OBJECT
public:
    explicit CompanionUdpDiscovery(unsigned discovery_port, QObject *parent = nullptr);
    explicit CompanionUdpDiscovery(unsigned discovery_port, QString discovery_name, QObject *parent = nullptr);
    virtual ~CompanionUdpDiscovery();

    void setDiscoveryPort(unsigned);
    unsigned getDiscoveryPort() const;

    void setDiscoveryName(const QString&);
    const QString& getDiscoveryName() const;

signals:

public slots:

protected slots:
    virtual void broadcast();

private:
    unsigned discovery_port_;
    QString discovery_name_;
};

#endif // COMPANION_UDP_DISCOVERY_H
