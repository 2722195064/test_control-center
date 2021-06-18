#ifndef NETWORKMODULE_H
#define NETWORKMODULE_H

#include "interfaces/moduleinterface.h"
#include "namespace.h"
#include "networkmodulewidget.h"

namespace dcc {
namespace network {
class NetworkModel;
class NetworkWorker;
class NetworkDevice;
}
}

namespace DCC_NAMESPACE {
class MainWindow;
namespace network {
class NetworkModule : public QObject, public ModuleInterface
{
    Q_OBJECT
public:
    explicit NetworkModule(FrameProxyInterface *frame, QObject *parent = nullptr);
    ~NetworkModule();

private:
    void preInitialize(bool sync = false , FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) override;
    void initialize() override;
    const QString name() const override;

public:
    void active() override;

private Q_SLOTS:
    void showDeviceDetailPage(dcc::network::NetworkDevice *dev, const QString &searchPath = "");
    void onDeviceListChanged(const QList<dcc::network::NetworkDevice *> &devices);


private:
    dcc::network::NetworkModel *m_networkModel;
    dcc::network::NetworkWorker *m_networkWorker;
    NetworkModuleWidget *m_networkWidget;
    MainWindow *m_pMainWindow;

    bool m_hasWired = false;
    bool m_hasWireless = false;
};
}
}

#endif // NETWORKMODULE_H
