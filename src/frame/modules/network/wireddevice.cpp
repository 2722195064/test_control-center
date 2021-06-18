#include "wireddevice.h"

using namespace dcc::network;

WiredDevice::WiredDevice(const QJsonObject &info, QObject *parent)
    : NetworkDevice (NetworkDevice::Wired, info, parent)
{

}

void WiredDevice::setConnections(const QList<QJsonObject> &connections)
{
    m_connections = connections;

    Q_EMIT connectionsChanged(connections);
}

void WiredDevice::setActiveConnectionsInfo(const QList<QJsonObject> &activeConnInfoList)
{

}
