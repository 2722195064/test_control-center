#include "device.h"

#include <QMap>
#include <QObject>
#include <QDebug>

namespace dcc {
namespace bluetooth {

Device::Device(QObject *parent)
    : QObject (parent)
    , m_id("")
    , m_name("")
    , m_paired(false)
    , m_trusted(false)
    , m_connecting(false)
    , m_connectState(false)
    , m_state(StateUnavailable)
{

}

void Device::setId(const QString &id)
{
    m_id = id;
}

void Device::setAddress(const QString &addr)
{
    m_address = addr;
}

void Device::setName(const QString &name)
{
    if (name != m_name) {
        m_name = name;
        Q_EMIT nameChanged(name);
    }
}

void Device::setAlias(const QString &alias)
{
    if (alias != m_alias) {
        m_alias = alias;
        Q_EMIT aliasChanged(alias);
    }
}

void Device::setPaired(bool paired)
{
    if (paired != m_paired) {
        qDebug() << "蓝牙设备 配对状态 ! ";
        m_paired = paired;
        Q_EMIT pairedChanged(paired);
    }
}

void Device::setState(const State &state, bool connectState)
{
    if ((state != m_state) || (connectState != m_connectState)) {
        m_state = state;
        m_connectState = connectState;
        Q_EMIT stateChanged(state, connectState);
    }
}

void Device::setTrusted(bool trusted)
{
    if (trusted != m_trusted) {
        m_trusted = trusted;
        Q_EMIT trustedChanged(trusted);
    }
}

void Device::setConnecting(bool connecting)
{
    if (connecting != m_connecting) {
        m_connecting = connecting;
        Q_EMIT connectingChanged(connecting);
    }
}

void Device::setDeviceType(const QString &deviceType)
{
    m_deviceType = deviceType2Icon[deviceType];
}

bool Device::canSendFile() const
{
    // 目前pc和手机可以发送蓝牙文件
    if ((m_deviceType == "pc") || (m_deviceType == "phone")) {
        return true;
    }
    return false;
}

} // namespace bluetooth
} // namespace dcc

