#include "networkdevice.h"

#include <QDebug>

using namespace dcc::network;

NetworkDevice::NetworkDevice(const DeviceType type, const QJsonObject &info, QObject *parent)
    : QObject (parent)
    , m_type(type)
    , m_deviceInfo(info)
{

}

NetworkDevice::~NetworkDevice()
{

}

const QString NetworkDevice::path() const
{

}

const QString NetworkDevice::realHwAdr() const
{
    qDebug() << "设备硬件地址: " << m_deviceInfo.value("HwAddress").toString();
    return m_deviceInfo.value("HwAddress").toString();
}

const QString NetworkDevice::interfaceName() const
{
    return m_deviceInfo.value("Interface").toString();
}

void NetworkDevice::updateDeviceInfo(const QJsonObject &devInfo)
{

}
