#include "networkworker.h"
#include <QMetaProperty>

using namespace dcc::network;

const QString networkService = "com.deepin.daemon.Network";
const QString networkPath = "/com/deepin/daemon/Network";

NetworkWorker::NetworkWorker(NetworkModel *model, QObject *parent, bool sync)
    : QObject (parent)
    , m_networkInter(networkService, networkPath, QDBusConnection::sessionBus(), this)
    , m_chainsInter(new ProxyChains(networkService, "/com/deepin/daemon/Network/ProxyChains", QDBusConnection::sessionBus(), this))
    , m_networkModel(model)
{
    // 监听网络适配器
    connect(&m_networkInter, &NetworkInter::DevicesChanged, m_networkModel, &NetworkModel::onDevicesChanged);

    connect(&m_networkInter, &NetworkInter::ConnectionsChanged, m_networkModel, &NetworkModel::onConnectionListChanged);

    m_networkInter.setSync(true);

    active(sync);
}

void NetworkWorker::active(bool bSync)
{
    m_networkInter.blockSignals(false);

    // 若需要同步获取信息
    if (bSync) {
        QDBusInterface inter(networkService, networkPath, networkService, QDBusConnection::sessionBus());
        QVariant req = inter.property("Devices");
        m_networkModel->onDevicesChanged(req.toString());
    } else {
        m_networkModel->onDevicesChanged(m_networkInter.devices());
    }
    m_networkModel->onConnectionListChanged(m_networkInter.connections());
    m_networkModel->onVPNEnabledChanged(m_networkInter.vpnEnabled());
    m_networkModel->onActiveConnectionsChanged(m_networkInter.activeConnections());
    m_networkModel->onConnectivityChanged(static_cast<int>(m_networkInter.connectivity()));

}

void NetworkWorker::queryActiveConnInfo()
{
    qDebug() << "查询连接信息: ";
    m_networkModel->onConnectivityChanged(static_cast<int>(m_networkInter.connectivity()));
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_networkInter.GetActiveConnectionInfo(), this);
    connect(w, &QDBusPendingCallWatcher::finished, this, &NetworkWorker::queryActiveConnInfoCB);
}

void NetworkWorker::queryActiveConnInfoCB(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = *w;
    m_networkModel->onActiveConnInfoChanged(reply.value());
    qDebug() << "获取网络连接信息: " << reply.value();
    w->deleteLater();
}

