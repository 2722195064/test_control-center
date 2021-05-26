#pragma once

#include <QObject>

#include <interfaces/moduleinterface.h>

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
class BluetoothWidget;
class BluetoothModule : public QObject, public ModuleInterface
{
    Q_OBJECT
public:
    explicit BluetoothModule(FrameProxyInterface *frame, QObject *parent = nullptr);
    void preInitialize(bool sync = false , FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) override;
    void initialize() override;
    const QString name() const override;
    void active() override;

private:
    ~BluetoothModule();

private:
    dcc::bluetooth::BluetoothWorker *m_bluetoothWork;
    dcc::bluetooth::BluetoothModel *m_bluetoothModel;
    BluetoothWidget *m_bluetoothWidget;

};
}
}
