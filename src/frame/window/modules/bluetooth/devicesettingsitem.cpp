#include "devicesettingsitem.h"

//using namespace dcc::widgets;
using namespace dcc;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::bluetooth;

DeviceSettingsItem::DeviceSettingsItem(const Device *device, QStyle *style)
    : m_device(device)
    , m_deviceItem(new DStandardItem)
    , m_style(style)
{
    initItemActionList();
    // TODO: setIcon

}

DeviceSettingsItem::~DeviceSettingsItem()
{
    if (!m_loadingIndicator.isNull()) {
        m_loadingIndicator->hide();
        m_loadingIndicator->deleteLater();
    }
    if (!m_loadingAction.isNull()) {
        m_loadingAction->setVisible(false);
    }
}

DStandardItem *DeviceSettingsItem::getStandardItem(DListView *parent)
{
    if (parent != nullptr) {
        m_parentDListView = parent;
        m_loadingIndicator->setParent(parent->viewport());

        // 设置每个Item 对应Device 信息
        if (m_device)
            setDevice(m_device);
    }
    return m_deviceItem;
}

const Device *DeviceSettingsItem::device() const
{
    return m_device;
}

void DeviceSettingsItem::initItemActionList()
{
    if (!m_loadingIndicator.isNull()) {
        m_loadingIndicator->deleteLater();
    }
    m_loadingIndicator = new DSpinner();
    m_loadingIndicator->setFixedSize(24, 24);
    m_loadingIndicator->hide();
    m_loadingAction = new DViewItemAction(Qt::AlignLeft | Qt::AlignCenter, QSize(), QSize(), false);
    m_loadingAction->setWidget(m_loadingIndicator);
    m_iconAction = new DViewItemAction(Qt::AlignCenter | Qt::AlignRight, QSize(), QSize(), true);
    m_textAction = new DViewItemAction(Qt::AlignLeft, QSize(), QSize(), true);
    m_spaceAction = new DViewItemAction(Qt::AlignCenter | Qt::AlignRight, QSize(), QSize(), false);
    m_iconAction->setIcon(m_style->standardIcon(QStyle::SP_ArrowRight));
    m_dActionList.clear();
    m_dActionList.append(m_loadingAction);
    m_dActionList.append(m_textAction);
    m_dActionList.append(m_spaceAction);
    m_dActionList.append(m_iconAction);

    m_deviceItem->setText(m_device->alias().isEmpty() ? m_device->name() : m_device->alias());
    m_deviceItem->setActionList(Qt::RightEdge, m_dActionList);
}

void DeviceSettingsItem::setDevice(const Device *device)
{
    // 配置需要更新的 Item 信息

    onDeviceStateChanged(device->state(), device->connectState());
    onDevicePairedChanged(device->paired());
}

void DeviceSettingsItem::onDeviceStateChanged(const Device::State &state, bool paired)
{

}

void DeviceSettingsItem::onDevicePairedChanged(const bool &paired)
{

}
