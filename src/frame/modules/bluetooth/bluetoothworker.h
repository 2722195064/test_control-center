#ifndef BLUETOOTHWORKER_H
#define BLUETOOTHWORKER_H

#include <QObject>

#include <com_deepin_daemon_bluetooth.h>

#include "bluetoothmodel.h"
using  DBusBluetooth = com::deepin::daemon::Bluetooth;

namespace dcc {
namespace bluetooth {

class BluetoothWorker : public QObject
{
    Q_OBJECT
public:
    static BluetoothWorker &Instance(bool sync = false);

    BluetoothModel *model() { return m_model; }

private:
    explicit BluetoothWorker(BluetoothModel *model, bool sync = false);
    ~BluetoothWorker();

public Q_SLOTS:
    void onAdapterPropertiesChanged(const QString &json);
    void onDevicePropertiesChanged(const QString &json);

    void addAdapter(const QString &json);
    void removeAdapter(const QString &json);

    void addDevice(const QString &json);
    void removeDevice(const QString &json);
    //设置适配器是否发现
    void setAdapterDiscovering(const QDBusObjectPath &path, bool enable);
    void setAdapterPowered(const Adapter *adapter, const bool &powered);

    void connectDevice(const Device *device, const Adapter *adapter);
    void disconnectDevice(const Device *device);
    void ignoreDevice(const Adapter *adapter, const Device *device);

    void refresh(bool beFirst = false);

private:
    // 当有蓝牙适配器后, 加载 获得的JSON数据
    void inflateAdapter(Adapter *adapter, const QJsonObject &adapterObj);
    void inflateDevice(Device *device, const QJsonObject &deviceObj);

Q_SIGNALS:
    void deviceEnableChanged();

private:
    DBusBluetooth *m_bluetoothInter;
    BluetoothModel *m_model;

};

} // namespace bluetooth
} // namespace dcc

#endif // BLUETOOTHWORKER_H
