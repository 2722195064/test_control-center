#include "bluetoothmodule.h"
#include "bluetoothwidget.h"

#include "modules/bluetooth/bluetoothworker.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::bluetooth;
using namespace dcc;
using namespace dcc::bluetooth;

BluetoothModule::BluetoothModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
{

}

void BluetoothModule::preInitialize(bool sync, FrameProxyInterface::PushType)
{
    m_bluetoothWork = &BluetoothWorker::Instance(sync);
    m_bluetoothModel = m_bluetoothWork->model();
    m_bluetoothModel->moveToThread(qApp->thread());
    m_bluetoothWork->moveToThread(qApp->thread());
    // lamda 函数实时监控适配器数量
    auto updateModuleVisible = [ = ] {
        // 实时检测蓝牙适配器的数量 以控制蓝牙模块的显示
        qDebug() << "适配器的数量: " << m_bluetoothModel->adapters().size();
//        m_frameProxy->setModuleVisible(this, m_bluetoothModel->adapters().size());
    };
    connect(m_bluetoothModel, &BluetoothModel::adpaterListChanged, this, updateModuleVisible);
    updateModuleVisible();
}

void BluetoothModule::initialize()
{

}

const QString BluetoothModule::name() const
{
    return QStringLiteral("bluetooth");
}

void BluetoothModule::active()
{
    m_bluetoothWidget = new BluetoothWidget(m_bluetoothModel);
//    m_bluetoothWidget->setAttribute(Qt::WA_StyledBackground,true);
//    m_bluetoothWidget->setStyleSheet("background-color: rgb(5, 255, 5)");

    m_bluetoothWidget->setModel(m_bluetoothModel);
    m_frameProxy->pushWidget(this, m_bluetoothWidget);


    m_bluetoothWidget->setVisible(true);
}

BluetoothModule::~BluetoothModule()
{

}
