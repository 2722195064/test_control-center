#ifndef NETWORKDEVICE_H
#define NETWORKDEVICE_H

#include <QJsonObject>
#include <QObject>

namespace dcc {
namespace network {
class NetworkDevice : public QObject
{
    Q_OBJECT
public:
    enum DeviceType
    {
        None,
        Wired,
        Wireless,
    };
public:
    ~NetworkDevice();
    DeviceType type() const { return m_type; }
    const QString path() const;
    const QString realHwAdr() const;
    const QString interfaceName() const;

public Q_SLOTS:
    void updateDeviceInfo(const QJsonObject &devInfo);

protected:
    explicit NetworkDevice(const DeviceType type, const QJsonObject &info, QObject *parent = nullptr);

private:
    const DeviceType m_type;
    QJsonObject m_deviceInfo;
    bool m_enabled;
};

}
}

#endif // NETWORKDEVICE_H
