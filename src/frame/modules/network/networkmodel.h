#ifndef NETWORKMODEL_H
#define NETWORKMODEL_H

#include "networkdevice.h"

#include <QMap>
#include <QObject>

namespace dcc {
namespace network {

// 标记位 判断网络连接是否有效
enum InterfaceFlags
{
    NM_DEVICE_INTERFACE_FLAG_NONE     = 0,
    NM_DEVICE_INTERFACE_FLAG_UP       = 0x1,
    NM_DEVICE_INTERFACE_FLAG_LOWER_UP = 0x2,
    NM_DEVICE_INTERFACE_FLAG_CARRIER  = 0x10000,
};

class NetworkModel : public QObject
{
    Q_OBJECT
public:
    explicit NetworkModel(QObject *parent = nullptr);
    ~NetworkModel();

    const QList<NetworkDevice *> devices() const { return m_devices; }


public Q_SLOTS:
    void onDevicesChanged(const QString &devices);
    void onConnectionListChanged(const QString &conns);
    void onVPNEnabledChanged(const bool enabled);
    void onActiveConnectionsChanged(const QString &conns);
    void onConnectivityChanged(int connectivity);
    void onActiveConnInfoChanged(const QString &conns);


private:
    NetworkDevice *device(const QString &devPath) const;

Q_SIGNALS:
    void requestDeviceStatus(const QString &devPath) const;
    void deviceListChanged(const QList<NetworkDevice *> devices) const;
    void connectionListChanged() const;

private:
    static QStringList m_deviceInterface;
    QList<NetworkDevice *> m_devices;

    QList<QJsonObject> m_activeConnInfos;
    QList<QJsonObject> m_activeConns;
    QMap<QString, QList<QJsonObject>> m_connections;

};

}
}

#endif // NETWORKMODEL_H
