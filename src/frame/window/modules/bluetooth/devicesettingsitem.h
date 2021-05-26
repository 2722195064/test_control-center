#ifndef DEVICESETTINGSITEM_H
#define DEVICESETTINGSITEM_H
#include"namespace.h"

#include <DListView>
#include <DSpinner>
#include <DStandardItem>
#include <QObject>
#include <QPointer>

#include <modules/bluetooth/device.h>

DWIDGET_USE_NAMESPACE
using namespace dcc::bluetooth;

QT_BEGIN_NAMESPACE
class QStyle;
QT_END_NAMESPACE

namespace dcc {
namespace bluetooth {
class Adapter;
}
}

namespace DCC_NAMESPACE {
namespace bluetooth {

class DeviceSettingsItem : public QObject
{
    Q_OBJECT
public:
    explicit DeviceSettingsItem(const dcc::bluetooth::Device *device, QStyle *style);
    virtual ~DeviceSettingsItem();

    DStandardItem *getStandardItem(DTK_WIDGET_NAMESPACE::DListView *parent = nullptr);
    const dcc::bluetooth::Device *device() const;

private:
    void initItemActionList();
    void setDevice(const dcc::bluetooth::Device *device);

private Q_SLOTS:
    void onDeviceStateChanged(const dcc::bluetooth::Device::State &state, bool paired);
    void onDevicePairedChanged(const bool &paired);

private:
    const dcc::bluetooth::Device *m_device { nullptr };
    DStandardItem *m_deviceItem { nullptr };
    QPointer<DTK_WIDGET_NAMESPACE::DSpinner> m_loadingIndicator;  // 链接状态
    DTK_WIDGET_NAMESPACE::DListView *m_parentDListView;

    DViewItemActionList m_dActionList;
    QPointer<DViewItemAction> m_loadingAction;
    QPointer<DViewItemAction> m_textAction;
    QPointer<DViewItemAction> m_spaceAction;
    QPointer<DViewItemAction> m_iconAction;
    QStyle *m_style;

};

}
}

#endif // DEVICESETTINGSITEM_H
