#ifndef WIRELESSDEVICE_H
#define WIRELESSDEVICE_H

#include "networkdevice.h"

#include <QObject>

namespace dcc {
namespace network {
class WirelessDevice : public NetworkDevice
{
    Q_OBJECT
public:
    explicit WirelessDevice(const QJsonObject &info, QObject *parent = nullptr);

public Q_SLOTS:
    void setConnections(const QList<QJsonObject> &connections);
    void setHotspotConnections(const QList<QJsonObject> &hotspotConnections);


};

}
}

#endif // WIRELESSDEVICE_H
