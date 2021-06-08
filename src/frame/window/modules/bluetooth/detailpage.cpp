#include "detailpage.h"

#include <DIconButton>
#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>

#include <widgets/titlelabel.h>
#include <widgets/translucentframe.h>

#include <modules/bluetooth/adapter.h>
#include <modules/bluetooth/device.h>

using namespace dcc;
using namespace dcc::bluetooth;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::bluetooth;

DetailPage::DetailPage(const Adapter *adapter, const Device *device)
    : ContentWidget()
    , m_adapter(adapter)
    , m_device(device)
    , m_ignoreButton(new QPushButton("忽略此设备",this))
    , m_disconnectButton(new QPushButton("断开连接",this))
    , m_connectButton(new QPushButton("连接",this))
{
    m_devNameLabel = new TitleLabel(device->name());

    dcc::widgets::TranslucentFrame *frame = new dcc::widgets::TranslucentFrame;
    QVBoxLayout *layout = new QVBoxLayout(frame);
    layout->setSpacing(0);
    layout->setContentsMargins(8, 8, 8, 8);

    DIconButton *backWidgetBtn = new DIconButton(this);
    backWidgetBtn->setAccessibleName("back_btn");
    backWidgetBtn->setFlat(true);
    backWidgetBtn->setFixedSize(30, 48);
    backWidgetBtn->setIcon(QStyle::SP_ArrowBack);
    DStyle::setFocusRectVisible(backWidgetBtn, false);
    layout->addWidget(backWidgetBtn, Qt::AlignLeft);

    m_devNameLabel = new TitleLabel(device->name());
    layout->addWidget(m_devNameLabel, 0, Qt::AlignCenter);
    layout->setSpacing(10);
    layout->addWidget(m_disconnectButton);
    layout->addWidget(m_connectButton);
    layout->addWidget(m_ignoreButton);
    layout->addStretch();
    setContent(frame);

    onDeviceStatusChanged();

    connect(m_device, &Device::stateChanged, this, [this] {
        onDeviceStatusChanged();
    });

    connect(m_ignoreButton, &QPushButton::clicked, this, [this] {
        Q_EMIT requestIgnoreDevice(m_adapter, m_device);
        if (QApplication::focusWidget()) {
            QApplication::focusWidget()->clearFocus();
        }
    });
    connect(m_disconnectButton, &QPushButton::clicked, this, [this] {
        Q_EMIT requestDisconnectDevice(m_device);
        if (QApplication::focusWidget()) {
            QApplication::focusWidget()->clearFocus();
        }
    });
    connect(m_connectButton, &QPushButton::clicked, this, [this] {
        Q_EMIT requestConnectDevice(m_device, m_adapter);
        if (QApplication::focusWidget()) {
            QApplication::focusWidget()->clearFocus();
        }
    });
}

void DetailPage::onDeviceStatusChanged()
{
    switch (m_device->state()) {
    case Device::StateConnected:
        if (m_device->connectState()) {
            m_disconnectButton->show();
            m_connectButton->hide();
            m_ignoreButton->setEnabled(true);
        } else {
            m_connectButton->show();
            m_connectButton->setEnabled(true);
            m_disconnectButton->hide();
            m_ignoreButton->setEnabled(true);
        }
        break;
    case Device::StateAvailable:
        m_connectButton->show();
        m_connectButton->setDisabled(true);
        m_disconnectButton->hide();
        m_ignoreButton->setEnabled(false);
        break;
    case Device::StateDisconnecting:
        m_connectButton->show();
        m_connectButton->setText("断开中");
        m_connectButton->setDisabled(true);
        m_disconnectButton->hide();
        m_ignoreButton->setEnabled(true);
        break;
    case Device::StateUnavailable:
        m_connectButton->show();
        m_connectButton->setEnabled(true);
        m_disconnectButton->hide();
        m_ignoreButton->setEnabled(true);
        break;
    }
}
