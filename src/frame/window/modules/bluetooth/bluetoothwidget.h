#ifndef BLUETOOTHWIDGET_H
#define BLUETOOTHWIDGET_H

#include <QObject>
#include <namespace.h>
#include <QMap>
#include "widgets/contentwidget.h"
#include "adapterwidget.h"

namespace dcc {
namespace bluetooth {
class BluetoothModel;
class BluetoothWorker;
class Device;
class Adapter;
}
}

namespace DCC_NAMESPACE {
namespace bluetooth {
class AdapterWidget;
//class BluetoothWidget : public QWidget
class BluetoothWidget : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit BluetoothWidget(dcc::bluetooth::BluetoothModel *model);
    virtual ~BluetoothWidget();
    void setModel(dcc::bluetooth::BluetoothModel *model);

Q_SIGNALS:
    void requestModuleVisible(const bool visible) const;
    void requestSetToggleAdapter(const dcc::bluetooth::Adapter *adapter, const bool &toggled);

    void requestConnectDevice(const dcc::bluetooth::Device *device, const dcc::bluetooth::Adapter *adapter);
    void requestDisconnectDevice(const dcc::bluetooth::Device *device);
    void showDeviceDetail(const dcc::bluetooth::Adapter *adapter, const dcc::bluetooth::Device *device);

public Q_SLOTS:
    void addAdapter(const dcc::bluetooth::Adapter *adapter);
    void removeAdapter(const dcc::bluetooth::Adapter *adapter);

private:
    void setVisibleState();
    AdapterWidget *getAdapter(const dcc::bluetooth::Adapter *adapter);
    void updateWidget();

private:
    dcc::bluetooth::BluetoothModel *m_model;
    dcc::bluetooth::BluetoothWorker *m_bluetoothWorker;
    QFrame *m_tFrame;
    QMap<const dcc::bluetooth::Adapter *, AdapterWidget *> m_valueMap;
};

}
}

#endif // BLUETOOTHWIDGET_H
