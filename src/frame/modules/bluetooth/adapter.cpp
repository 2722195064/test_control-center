#include "adapter.h"
#include <QDebug>

namespace dcc {
namespace bluetooth {

Adapter::Adapter(QObject *parent)
    : QObject (parent)
{

}

void Adapter::setName(const QString &name)
{
    if (name != m_name) {
        m_name = name;
        Q_EMIT nameChanged(name);
    }
}

void Adapter::setId(const QString &id)
{
    m_id = id;
}

QMap<QString, const Device *> Adapter::devices() const
{
    return m_devices;
}

QList<QString> Adapter::devicesId() const
{
    return m_devicesId;
}

const Device *Adapter::deviceById(const QString &id) const
{
    return m_devices.keys().contains(id) ? m_devices[id] : nullptr;
}

void Adapter::setPowered(bool powered, bool discovering)
{
    if (powered != m_powered || (powered && m_powered && discovering != m_discovering)) {
        m_powered = powered;
        m_discovering = discovering;
        qDebug() << " 蓝牙开关状态 ! ";
        Q_EMIT poweredChanged(powered, discovering);
    }
}

void Adapter::setDiscoverabled(const bool discoverable)
{
    if (m_discoverable != discoverable) {
        m_discoverable = discoverable;
        qDebug() << " 此处蓝牙被发现状态改变! ";
        Q_EMIT discoverableChanged(discoverable);
    }
}

void Adapter::addDevice(const Device *device)
{
    if (!deviceById(device->id())){
        m_devicesId << device->id();
        m_devices[device->id()] = device;

        if (!device->name().isEmpty() && device->paired())
            qDebug() << "Adapter::addDevice 正在 配对设备:  " << device->name();
        Q_EMIT deviceAdded(device);
    }
}

void Adapter::removeDevice(const QString &deviceId)
{
    const Device *device = nullptr;

    device = deviceById(deviceId);
    if (device) {
        m_devicesId.removeOne(deviceId);
        m_devices.remove(deviceId);
        Q_EMIT deviceRemoved(deviceId);
    }
}


} // namespace bluetooth
} // namespace dcc
