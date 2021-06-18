#ifndef WIREDDEVICE_H
#define WIREDDEVICE_H

#include "networkdevice.h"

#include <QObject>

namespace dcc {
namespace network {
class WiredDevice : public NetworkDevice
{
    Q_OBJECT
public:
    explicit WiredDevice(const QJsonObject &info, QObject *parent = nullptr);

     void setConnections(const QList<QJsonObject> &connections);

     void setActiveConnectionsInfo(const QList<QJsonObject> &activeConnInfoList);

Q_SIGNALS:
     void connectionsChanged(const QList<QJsonObject> &connections) const;

private:
     QList<QJsonObject> m_connections;
     QList<QJsonObject> m_activeConnectionsInfo;

};

}
}

#endif // WIREDDEVICE_H
