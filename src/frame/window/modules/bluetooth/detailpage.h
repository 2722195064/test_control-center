#pragma once

#ifndef DETAILPAGE_H
#define DETAILPAGE_H

#include "namespace.h"
#include <QObject>

#include <widgets/contentwidget.h>
//#include <widgets/titlelabel.h>
class TitleLabel;

namespace dcc {
namespace bluetooth {
class Device;
class Adapter;
}
}

namespace DCC_NAMESPACE {
namespace bluetooth {
class DetailPage : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit DetailPage(const dcc::bluetooth::Adapter *adapter, const dcc::bluetooth::Device *device);

public Q_SLOTS:
    void removeDevice(const QString &id);

Q_SIGNALS:
    void requestIgnoreDevice(const dcc::bluetooth::Adapter *adapter, const dcc::bluetooth::Device *device);
    void requestDisconnectDevice(const dcc::bluetooth::Device *device);
    void requestConnectDevice(const dcc::bluetooth::Device *device, const dcc::bluetooth::Adapter *adapter);
    void requestBack();

private Q_SLOTS:
    void onDeviceStatusChanged();

private:
    const dcc::bluetooth::Adapter *m_adapter;
    const dcc::bluetooth::Device *m_device;
    TitleLabel *m_devNameLabel;
    QPushButton *m_ignoreButton;
    QPushButton *m_disconnectButton;
    QPushButton *m_connectButton;
};

}
}

#endif // DETAILPAGE_H
