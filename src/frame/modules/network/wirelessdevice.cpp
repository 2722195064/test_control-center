#include "wirelessdevice.h"

using namespace dcc::network;


WirelessDevice::WirelessDevice(const QJsonObject &info, QObject *parent)
    : NetworkDevice (NetworkDevice::Wired, info, parent)
{

}

void WirelessDevice::setConnections(const QList<QJsonObject> &connections)
{

}

void WirelessDevice::setHotspotConnections(const QList<QJsonObject> &hotspotConnections)
{

}
