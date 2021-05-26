#ifndef ADAPTERWIDGET_H
#define ADAPTERWIDGET_H

#include "titleedit.h"
#include <DSpinner>
#include <DIconButton>
#include <DCheckBox>
#include <DListView>

#include <QObject>
#include <QWidget>

#include <modules/bluetooth/bluetoothmodel.h>

#include <com_deepin_daemon_bluetooth.h>
using BluetoothInter = com::deepin::daemon::Bluetooth;

DWIDGET_USE_NAMESPACE

QT_BEGIN_NAMESPACE
class QLabel;
class QStandardItemModel;
QT_END_NAMESPACE

class TitleLabel;

namespace dcc {
namespace bluetooth {
class Device;
class Adapter;
}

namespace widgets {
class SwitchWidget;
class SettingsGroup;
}
}

namespace DCC_NAMESPACE {
namespace bluetooth {
class TitleEdit;
class DeviceSettingsItem;
class AdapterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AdapterWidget(const dcc::bluetooth::Adapter *adapter, dcc::bluetooth::BluetoothModel *model);
    virtual ~AdapterWidget();
    void setAdapter(const dcc::bluetooth::Adapter *adapter);

private:
    void initMember();
    void initUI();
    void initConnect();
    // 添加设备信息
    void categoryDevice(DeviceSettingsItem *deviceItem, const bool paired);

private Q_SLOTS:
    void addDevice(const dcc::bluetooth::Device *device);
    void onPowerStatus(bool bPower, bool bDiscovering);


private:
    TitleEdit *m_titleEdit;
    QLabel *m_hideAnonymousLabel;
    const dcc::bluetooth::Adapter *m_adapter;
    DCheckBox *m_showAnonymousCheckBox;  // 显示有名称的设备

    TitleLabel *m_myDevicesGroup;
    TitleLabel *m_otherDevicesGroup;
    DTK_WIDGET_NAMESPACE::DSpinner *m_spinner;
    QPointer<DTK_WIDGET_NAMESPACE::DSpinner> m_spinnerBtn;
    dcc::widgets::SwitchWidget *m_powerSwitch;
    DTK_WIDGET_NAMESPACE::DIconButton *m_refreshBtn;
    dcc::widgets::SwitchWidget *m_discoverySwitch;

    DTK_WIDGET_NAMESPACE::DListView *m_myDeviceListView;
    QStandardItemModel *m_myDeviceModel;
    QList<QPointer<DeviceSettingsItem>> m_myDevice;
    DTK_WIDGET_NAMESPACE::DListView *m_otherDeviceListView;
    QStandardItemModel *m_otherDeviceModel;
    QList<QPointer<DeviceSettingsItem>> m_deviceLists; // all

    bool m_showUnnamedDevices;
    dcc::bluetooth::BluetoothModel *m_model;
    BluetoothInter m_bluetoothInter;

    bool m_lastPowerCheck;
};

}
}


#endif // ADAPTERWIDGET_H
