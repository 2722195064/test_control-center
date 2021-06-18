#include "networkmodule.h"

#include "modules/network/networkmodel.h"
#include "modules/network/networkworker.h"
#include "connectioneditpage.h"


using namespace dcc;
using namespace dcc::network;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::network;

NetworkModule::NetworkModule(FrameProxyInterface *frame, QObject *parent)
    : QObject (parent)
    , ModuleInterface (frame)
    , m_networkModel(nullptr)
    , m_networkWorker(nullptr)
    , m_networkWidget(nullptr)
{
//    ConnectionEditPage::setFrameProxy(frame);
//    m_pMainWindow = dynamic_cast<MainWindow *>(m_frameProxy);
    m_hasWired = false;
    m_hasWireless = false;
}

NetworkModule::~NetworkModule()
{

}

void NetworkModule::preInitialize(bool sync, FrameProxyInterface::PushType)
{
    m_networkModel = new NetworkModel;
    m_networkWorker = new NetworkWorker(m_networkModel, nullptr, sync);

    m_networkModel->moveToThread(qApp->thread());
    m_networkWorker->moveToThread(qApp->thread());

    connect(m_networkModel, &NetworkModel::deviceListChanged, this, &NetworkModule::onDeviceListChanged);
    onDeviceListChanged(m_networkModel->devices());
}

void NetworkModule::initialize()
{

}

const QString NetworkModule::name() const
{
    return QString("网络");
}

void NetworkModule::active()
{
    m_networkWidget = new NetworkModuleWidget;
    m_networkWidget->setVisible(false);
    m_networkWidget->setModel(m_networkModel);

    m_frameProxy->pushWidget(this, m_networkWidget);
    m_networkWidget->setVisible(true);
}

void NetworkModule::showDeviceDetailPage(NetworkDevice *dev, const QString &searchPath)
{
    
}

void NetworkModule::onDeviceListChanged(const QList<NetworkDevice *> &devices)
{

}
