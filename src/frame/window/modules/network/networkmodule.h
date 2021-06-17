#ifndef NETWORKMODULE_H
#define NETWORKMODULE_H

#include "interfaces/moduleinterface.h"
#include "namespace.h"
#include "networkmodulewidget.h"

namespace dcc {
namespace network {
class NetworkModel;
class NetworkWorker;
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

private:
    dcc::network::NetworkModel *m_networkModel;
    dcc::network::NetworkWorker *m_networkWorker;
    NetworkModuleWidget *m_networkWidget;

};
}
}


#endif // NETWORKMODULE_H
