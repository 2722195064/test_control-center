#include "networkmodule.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::network;

NetworkModule::NetworkModule(FrameProxyInterface *frame, QObject *parent)
    : QObject (parent)
    , ModuleInterface (frame)
    , m_networkModel(nullptr)
    , m_networkWorker(nullptr)
    , m_networkWidget(nullptr)
{

}

NetworkModule::~NetworkModule()
{

}

void NetworkModule::preInitialize(bool sync, FrameProxyInterface::PushType)
{

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

}
