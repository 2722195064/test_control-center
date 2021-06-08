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

    m_deviceItem->setText(m_device->alias().isEmpty() ? m_device->name() : m_device->alias());
    m_deviceItem->setActionList(Qt::RightEdge, m_dActionList);
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

DStandardItem *DeviceSettingsItem::createStandardItem(DListView *parent)
{
    initItemActionList();
    if (parent != nullptr) {
        m_parentDListView = parent;
        m_loadingIndicator->setParent(parent->viewport());
        if (m_device)
            setDevice(m_device);
    }

    m_deviceItem = new DStandardItem;
    m_deviceItem->setText(m_device->alias().isEmpty() ? m_device->name() : m_device->alias());
    m_deviceItem->setActionList(Qt::RightEdge, m_dActionList);
    return m_deviceItem;
}

const Device *DeviceSettingsItem::device() const
{
    return m_device;
}

void DeviceSettingsItem::setLoading(const bool loading)
{
    if (loading) {
        onUpdateLoading();
    } else {
        loadingStop();
    }

    if (m_parentDListView)
        m_parentDListView->update();
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
}

void DeviceSettingsItem::setDevice(const Device *device)
{
    connect(device, &Device::stateChanged, this, &DeviceSettingsItem::onDeviceStateChanged);
    connect(device, &Device::pairedChanged, this, &DeviceSettingsItem::onDevicePairedChanged);

    // 配置需要更新的 Item 信息
    // TODO: 新增每个Item对应功能
    connect(m_textAction, &QAction::triggered, this, [this] {
        for (int i = 0; i < m_parentDListView->count(); i++) {
            const QStandardItemModel *deviceModel = dynamic_cast<const QStandardItemModel *>(m_parentDListView->model());
            if (!deviceModel) {
                return ;
            }
            DStandardItem *item = dynamic_cast<DStandardItem *>(deviceModel->item(i));
            if (!item)
                return;

            if (m_deviceItem == item) {
                m_parentDListView->setCurrentIndex(m_parentDListView->model()->index(i, 0));
                m_parentDListView->clicked(m_parentDListView->model()->index(i, 0));
                break;
            }
        }
    });
    // 显示请求页
    connect(m_iconAction, &QAction::triggered, this, [=]{
        Q_EMIT requestShowDetail(m_device);
    });

    onDeviceStateChanged(device->state(), device->connectState());
    onDevicePairedChanged(device->paired());
}

void DeviceSettingsItem::loadingStart()
{
    m_loadingIndicator->start();
    m_loadingIndicator->show();
    m_loadingAction->setVisible(true);
    m_textAction->setVisible(false);
}

void DeviceSettingsItem::loadingStop()
{
    m_loadingIndicator->stop();
    m_loadingIndicator->hide();
    m_loadingAction->setVisible(false);
    m_textAction->setVisible(true);
}

void DeviceSettingsItem::onDeviceStateChanged(const Device::State &state, bool paired)
{
    if (state == Device::StateAvailable) {
        setLoading(true);
        return;
    }

    QString tip;
    if (state == Device::StateConnected && paired) {
        tip = ("已连接");
        setLoading(false);
    } else if (state == Device::StateUnavailable || state == Device::StateDisconnecting) {
        tip = ("未连接");
        setLoading(false);
    }
    m_textAction->setText(tip);
}

void DeviceSettingsItem::onDevicePairedChanged(const bool &paired)
{
    m_iconAction->setVisible(paired);
    m_spaceAction->setVisible(paired);
}

void DeviceSettingsItem::onUpdateLoading()
{
    // 确保每次 item 中的 spinner 是在对应item框内的
    if (m_parentDListView) {
        QModelIndex index;
        DStandardItem *item;
        for (int i = 0; i < m_parentDListView->count(); i++) {
            const QStandardItemModel *deviceModel = dynamic_cast<const QStandardItemModel *>(m_parentDListView->model());
            if (!deviceModel)
                return;

            item = dynamic_cast<DStandardItem *>(deviceModel->item(i));
            if (!item)
                continue;
            if (m_deviceItem == item) {
                index = m_parentDListView->model()->index(i, 0);
                break;
            }
        }
        // 获取对应item框的范围
        QRect itemRect = m_parentDListView->visualRect(index);
        if (item && (itemRect.height() != 0 || index.row() >= 1)) {
            QPoint point(itemRect.x() + itemRect.width(), itemRect.y());
            m_loadingIndicator->move(point);
            loadingStart();
            return;
        }

        if (m_device->state() != Device::StateAvailable)
            loadingStop();
        else {
            loadingStart();
        }
    }
}
