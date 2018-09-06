#include "companion_udp_discovery.h"

#include "network_message.h"

CompanionUdpDiscovery::CompanionUdpDiscovery(unsigned discovery_port, QObject *parent)
    : UdpBroadcastServer(parent)
    , discovery_port_(discovery_port)
    , discovery_name_("Companion Host")
{
}

CompanionUdpDiscovery::CompanionUdpDiscovery(unsigned discovery_port, QString discovery_name, QObject *parent)
    : UdpBroadcastServer(parent)
    , discovery_port_(discovery_port)
    , discovery_name_(discovery_name)
{}

CompanionUdpDiscovery::~CompanionUdpDiscovery()
{}

void CompanionUdpDiscovery::setDiscoveryPort(unsigned p)
{
    discovery_port_ = p;
}

unsigned CompanionUdpDiscovery::getDiscoveryPort() const
{
    return discovery_port_;
}

void CompanionUdpDiscovery::setDiscoveryName(const QString &name)
{
    discovery_name_ = name;
}

const QString &CompanionUdpDiscovery::getDiscoveryName() const
{
    return discovery_name_;
}

void CompanionUdpDiscovery::broadcast()
{
    QJsonObject o;
    o["port"] = QString::number(discovery_port_);
    o["name"] = discovery_name_;
    NetworkMessage msg("I serve Companion", o);
    socket_->writeDatagram(msg.toByteArray(), QHostAddress::Broadcast, port_);
}
