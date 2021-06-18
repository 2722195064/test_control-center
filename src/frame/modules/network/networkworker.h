#ifndef NETWORKWORKER_H
#define NETWORKWORKER_H

#include "networkmodel.h"

#include <QObject>

#include <com_deepin_daemon_network.h>
#include <com_deepin_daemon_network_proxychains.h>

namespace dcc {
namespace network {

using NetworkInter = com::deepin::daemon::Network;
using ProxyChains = com::deepin::daemon::network::ProxyChains;

class NetworkWorker : public  QObject
{
    Q_OBJECT

public:
    explicit NetworkWorker(NetworkModel *model, QObject *parent = nullptr, bool sync = false);

    void active(bool bSync = false);

public Q_SLOTS:
    void queryActiveConnInfo();

private Q_SLOTS:
    void queryActiveConnInfoCB(QDBusPendingCallWatcher *w);

private:
    NetworkInter m_networkInter;
    ProxyChains *m_chainsInter;
    NetworkModel *m_networkModel;

};

}
}

#endif // NETWORKWORKER_H
