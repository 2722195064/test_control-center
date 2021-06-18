#include "networkdevice.h"
#include "networkmodel.h"
#include "wireddevice.h"
#include "wirelessdevice.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>
#include <QDebug>

using namespace dcc;
using namespace dcc::network;

QStringList NetworkModel::m_deviceInterface;

NetworkDevice::DeviceType parseDeviceType(const QString &type)
{
    if (type == "wireless") {
        return NetworkDevice::Wireless;
    }
    if (type == "wired") {
        return NetworkDevice::Wired;
    }

    return NetworkDevice::None;
}

NetworkModel::NetworkModel(QObject *parent)
    : QObject (parent)
{

}

NetworkModel::~NetworkModel()
{

}

void NetworkModel::onDevicesChanged(const QString &devices)
{
    const QJsonObject data = QJsonDocument::fromJson(devices.toUtf8()).object();
    QSet<QString> devSet;
    bool changed = false;
    m_deviceInterface.clear();

    for (auto it(data.constBegin()); it != data.constEnd(); ++it) {
        const auto type = parseDeviceType(it.key());
        const auto list = it.value().toArray();

        if (type == NetworkDevice::None)
            continue;

        for (auto const &l : list) {
            const auto info = l.toObject();
            const QString path = info.value("Path").toString();
            const bool managed = info.value("Managed").toBool();
            QString interface = info.value("Interface").toString();
            qDebug() << " interface ==> " << interface;

            if (!m_deviceInterface.contains(interface)) {
                m_deviceInterface << interface;
            }

            if (!managed) {
                qDebug() << " Device : " << path << "未管理!";
                continue;
            }
            // 根据标志位 InterfaceFlags 判断网络连接是否有效
            if (type != NetworkDevice::Wireless) {
                if (!info.value("InterfaceFlays").isUndefined()) {
                    // 网络有效
                    int flag = info.value("InterfaceFlays").toInt();
                    if (!(flag & NM_DEVICE_INTERFACE_FLAG_UP)) {
                        continue;
                    }
                }
            }

            if (!devSet.contains(path))
                devSet << path;

            // 区分设备信息, 更新设备状态
            NetworkDevice *d = device(path);
            if (d == nullptr) {
                changed = true;
                switch (type) {
                    case NetworkDevice::Wireless: d = new WirelessDevice(info, this); break;
                    case NetworkDevice::Wired:    d = new WiredDevice(info, this);    break;
                    default:;
                }

                m_devices.append(d);
                if (d != nullptr) {
                    Q_EMIT requestDeviceStatus(d->path());
                }
            } else {
                d->updateDeviceInfo(info);
            }

        }
    }

    // 移除未列出的设备
    QList<NetworkDevice *> removeList;
    for (auto const d : m_devices) {
        if (!devSet.contains(d->path()))
            removeList << d;
    }
    for (auto const r : removeList) {
        m_devices.removeOne(r);
        r->deleteLater();
    }
    if(!removeList.isEmpty())
        changed = true;

    if (changed)
        Q_EMIT deviceListChanged(m_devices);
}

void NetworkModel::onConnectionListChanged(const QString &conns)
{
    qDebug() << __FILE__ << __func__ << " All connection : ";
    QMap<QString, QList<QJsonObject>> commonConnections;
    QMap<QString, QMap<QString, QList<QJsonObject>>> deviceConnections;

    QMap<QString, QList<QJsonObject>> wiredCommonConnections;
    QMap<QString, QMap<QString, QList<QJsonObject>>> wiredDeviceConnections;

    // 解析所有 connection
    const QJsonObject connsObj = QJsonDocument::fromJson(conns.toUtf8()).object();
    for (auto it(connsObj.constBegin()); it != connsObj.constEnd(); it++) {
        const auto &connList = it.value().toArray();
        const auto &connType = it.key();
        if (connType.isEmpty())
            continue;

        m_connections[connType].clear();
        // 重新导入所有 连接数据value
        for (const auto &connObj : connList) {
            const QJsonObject &connDate = connObj.toObject();
            m_connections[connType].append(connDate);

            const auto &hwAddr = connDate.value("HwAddress").toString();
            if (hwAddr.isEmpty()) {
                commonConnections[connType].append(connDate);
            } else {
                deviceConnections[hwAddr][connType].append(connDate);
            }

            const auto &interface = connDate.value("IfcName").toString();
            if (interface.isEmpty()) {
                wiredCommonConnections[connType].append(connDate);
            } else {
                wiredDeviceConnections[interface][connType].append(connDate);
            }
        }
    }

    // 将 connections 分配给具体设备
    for (NetworkDevice *dev : m_devices) {
        const QString &hwAddr = dev->realHwAdr(); // 硬件地址
        const QMap<QString, QList<QJsonObject>> &connsByType = deviceConnections.value(hwAddr);
        const QString &inter = dev->interfaceName();
        const QMap<QString, QList<QJsonObject>> &connsByInter = wiredDeviceConnections.value(inter);
        QList<QJsonObject> destConns;

        switch (dev->type()) {
        // 无线
        case NetworkDevice::Wireless: {
            destConns += commonConnections.value("wireless");
            destConns += connsByType.value("wireless");
            WirelessDevice *wsDevice = static_cast<WirelessDevice *>(dev);
            wsDevice->setConnections(destConns);

            destConns.clear();
            destConns += commonConnections.value("wireless-hotspot");
            destConns += connsByType.value("wireless-hotspot");
            wsDevice->setHotspotConnections(destConns);
            break;
        }
        case NetworkDevice::Wired:{
            destConns += wiredCommonConnections.value("wired");
            destConns += connsByInter.value("wired");
            WiredDevice *wdDevice = static_cast<WiredDevice *>(dev);
            wdDevice->setConnections(destConns);
            break;
        }
        default:
            break;
        }
    }
    Q_EMIT connectionListChanged();
}

void NetworkModel::onVPNEnabledChanged(const bool enabled)
{
    qDebug() << " NetworkModel::onVPNEnabledChanged ";

}

void NetworkModel::onActiveConnectionsChanged(const QString &conns)
{
    qDebug() << __FILE__ << __func__ << " All active connections : ";
    m_activeConnInfos.clear();

}

void NetworkModel::onConnectivityChanged(int connectivity)
{
    qDebug() << " NetworkModel::onConnectivityChanged ";

}

void NetworkModel::onActiveConnInfoChanged(const QString &conns)
{
    qDebug() << __FILE__ << "解析后端 GetActiveConnectionInfo 数据:";
    m_activeConnInfos.clear();
    QMap<QString, QJsonObject> activeConnInfo;
    QMap<QString, QJsonObject> activeHotspotInfo;

    QJsonArray activeConns = QJsonDocument::fromJson(conns.toUtf8()).array();
    for (const auto &info : activeConns) {
        const auto &connInfo = info.toObject();
        const auto &type = connInfo.value("ConnectionType").toString();
        const auto &devPath = connInfo.value("Device").toString();

        activeConnInfo.insertMulti(devPath, connInfo);
        m_activeConnInfos << connInfo;

        if (type == "wireless-hotspot") {
            activeHotspotInfo.insert(devPath, connInfo);
        }
    }

    // 更新主动连接的设备
//    for (auto *dev : m_devices) {
//        const   auto &devpath = dev->path();
//        switch (dev->type()) {
//        case NetworkDevice::Wired:
//        {
//            WiredDevice *d = static_cast<WiredDevice *>(dev);
//            d->setActiveConnectionsInfo(activeConnInfo.values(devPath));
//            break;
//        }
//        }
//    }
}

NetworkDevice *NetworkModel::device(const QString &devPath) const
{
    for (auto const d : m_devices) {
        if (d->path() == devPath)
            return d;
    }
    return nullptr;
}

