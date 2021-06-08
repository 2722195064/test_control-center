#include "bluetoothwidget.h"

#include "modules/bluetooth/bluetoothmodel.h"
#include "modules/bluetooth/bluetoothworker.h"

#include <QVBoxLayout>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::bluetooth;
using namespace dcc;
using namespace dcc::bluetooth;
using namespace dcc::widgets;

BluetoothWidget::BluetoothWidget(BluetoothModel *model)
    : ContentWidget(nullptr)
    , m_model(model)
{
    layout()->setContentsMargins(0, 8, 0, 8);
    m_tFrame = new QFrame;
    setContent(m_tFrame);
//    this->setAttribute(Qt::WA_StyledBackground,true);
//    this->setStyleSheet("background-color: rgb(255,255, 5)");
    m_bluetoothWorker = &dcc::bluetooth::BluetoothWorker::Instance();
}

BluetoothWidget::~BluetoothWidget()
{

}

void BluetoothWidget::setModel(BluetoothModel *model)
{
    model->disconnect(this);
    connect(model, &BluetoothModel::adapterAdded, this, &BluetoothWidget::addAdapter);
    connect(model, &BluetoothModel::adapterRemove, this, &BluetoothWidget::removeAdapter);

    qDebug() << "BluetoothWidget::setModel => size: " << model->adapters().size();
    for (const Adapter *adapter : model->adapters())
        addAdapter(adapter);
}

//
void BluetoothWidget::addAdapter(const Adapter *adapter)
{
    if (!m_valueMap.contains(adapter)) {
        QLayout *layout = m_tFrame->layout();

        if (!layout)
            layout = new QVBoxLayout;

        layout->addWidget(getAdapter(adapter));
        m_tFrame->setLayout(layout);

        setVisibleState();
    }
}

void BluetoothWidget::removeAdapter(const Adapter *adapter)
{
    if (m_valueMap.contains(adapter)) {
        QWidget *w = m_valueMap.take(adapter);
        m_tFrame->layout()->removeWidget(w);
        w->setParent(nullptr);
        w->deleteLater();
        setVisibleState();
        updateWidget();
    }
}

void BluetoothWidget::setVisibleState()
{
    Q_EMIT requestModuleVisible(m_valueMap.size());
}

AdapterWidget *BluetoothWidget::getAdapter(const Adapter *adapter)
{
    AdapterWidget *adapterWidget = new AdapterWidget(adapter, m_model);
//    adapterWidget->setAttribute(Qt::WA_StyledBackground,true);
//    adapterWidget->setStyleSheet("background-color: rgb(255, 5, 255)");
    const QDBusObjectPath path(adapter->id());

    // 交互
    connect(adapterWidget, &AdapterWidget::requestSetToggleAdapter, this, &BluetoothWidget::requestSetToggleAdapter);
    connect(adapterWidget, &AdapterWidget::notifyRefreshDevice, this, &BluetoothWidget::updateWidget);
    connect(adapterWidget, &AdapterWidget::requestConnectDevice, this, &BluetoothWidget::requestConnectDevice);
    connect(adapterWidget, &AdapterWidget::requestShowDetail, this, &BluetoothWidget::showDeviceDetail);

    m_bluetoothWorker->setAdapterDiscovering(path, true);
    m_valueMap[adapter] = adapterWidget;
    return  adapterWidget;
}

void BluetoothWidget::updateWidget()
{
    QLayout *layout = m_tFrame->layout();
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->setMargin(0);
    vLayout->setSpacing(0);
    if (layout){
        // 将久蓝牙设备放入新的 Layout 中
        while (QLayoutItem *item = layout->takeAt(0)) {
            if (!item->widget()){
                continue;
            }
            vLayout->addWidget(item->widget(), 0, Qt::AlignTop);
        }
        layout->setParent(nullptr);
        layout->deleteLater();
        layout = nullptr;
    }
    m_tFrame->deleteLater();
    m_tFrame = new QFrame;
    vLayout->addStretch();
    m_tFrame->setLayout(vLayout);
    setContent(m_tFrame);
}
