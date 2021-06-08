#include "adapterwidget.h"
#include "devicesettingsitem.h"

#include <widgets/settingsgroup.h>
#include <widgets/switchwidget.h>
#include <widgets/titlelabel.h>

using BluetoothInter = com::deepin::daemon::Bluetooth;

using namespace dcc;
using namespace dcc::bluetooth;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::bluetooth;

DWIDGET_USE_NAMESPACE

AdapterWidget::AdapterWidget(const Adapter *adapter, BluetoothModel *model)
    : m_titleEdit(new TitleEdit)
    , m_hideAnonymousLabel(new QLabel("显示没有名称的蓝牙设备"))
    , m_adapter(adapter)
    , m_showAnonymousCheckBox(new DCheckBox(this))
    , m_powerSwitch(new SwitchWidget(nullptr, m_titleEdit))
    , m_discoverySwitch(new SwitchWidget("允许其他蓝牙设备找到此设备"))
//    , m_lastPowerCheck(false)
    , m_model(model)
    , m_bluetoothInter("com.deepin.daemon.Bluetooth", "/com/deepin/daemon/Bluetooth", QDBusConnection::sessionBus(), this)
{
    initMember();
    initUI();
    initConnect();
    // 设置对应适配器信息
    QTimer::singleShot(0, this, [this]{
        setAdapter(m_adapter);
    });
}

AdapterWidget::~AdapterWidget()
{

}

void AdapterWidget::setAdapter(const Adapter *adapter)
{
    qDebug() << " ----- AdapterWidget::setAdapter  调整适配器的基本显示: ";

    // 添加设备到显示框
    connect(adapter, &Adapter::deviceAdded, this, &AdapterWidget::addDevice, Qt::QueuedConnection);
    connect(adapter, &Adapter::poweredChanged, this, &AdapterWidget::onPowerStatus, Qt::QueuedConnection);
    connect(adapter, &Adapter::deviceRemoved, this, &AdapterWidget::removeDevice, Qt::QueuedConnection);
    connect(adapter, &Adapter::recoveryStatus, m_powerSwitch, [ = ] {
        m_powerSwitch->switchButton()->setEnabled(true);
    });

    connect(adapter, &Adapter::loadStatus, m_powerSwitch, [ = ] (bool isOpen){
        qDebug() << "loadStatus isOpen = " << isOpen;
//        m_powerSwitch->switchButton()->setEnabled(isOpen);
//        m_spinnerBtn->setVisible(isOpen);
    });

    m_titleEdit->setTitle(adapter->name());
    for (QString id : adapter->devicesId()) {
        if (adapter->devices().contains(id)) {
            const Device *device = adapter->devices()[id];
            addDevice(device);
        }
    }
    m_lastPowerCheck = adapter->powered();
    qDebug() << "=== 初始化开关状态 : " << m_lastPowerCheck;
    m_discoverySwitch->setChecked(m_adapter->discoverabled());
    onPowerStatus(m_lastPowerCheck, adapter->discovering());
}

void AdapterWidget::initMember()
{
    m_showUnnamedDevices = m_bluetoothInter.displaySwitch();
}

void AdapterWidget::initUI()
{
    m_myDevicesGroup = new TitleLabel("我的设备");
    m_myDevicesGroup->setVisible(false);

    m_otherDevicesGroup = new TitleLabel("未链接设备");
    m_spinnerBtn = new DSpinner(m_titleEdit);

    m_titleEdit->setMinimumWidth(10);
    m_powerSwitch->getMainLayout()->insertWidget(1, m_spinnerBtn, Qt::AlignVCenter);

    m_spinnerBtn->setFixedSize(24, 24);
    m_spinnerBtn->start();
    m_spinnerBtn->hide();
    m_spinner = new DSpinner();
    m_spinner->setFixedSize(24, 24);
    m_spinner->start();
    m_refreshBtn = new DIconButton(this);
    m_refreshBtn->setFixedSize(36, 36);
    m_refreshBtn->setIcon(QIcon::fromTheme("dcc_refresh"));
    m_showAnonymousCheckBox->setChecked(m_showUnnamedDevices);

    m_hideAnonymousLabel->setFixedHeight(36);
    m_hideAnonymousLabel->setMinimumWidth(10);
    m_showAnonymousCheckBox->setFixedHeight(36);
    QHBoxLayout *phlayoutShowAnonymous = new QHBoxLayout;
    phlayoutShowAnonymous->addWidget(m_showAnonymousCheckBox);
    phlayoutShowAnonymous->addWidget(m_hideAnonymousLabel);
    phlayoutShowAnonymous->addStretch();
    phlayoutShowAnonymous->addWidget(m_spinner);
    phlayoutShowAnonymous->addWidget(m_refreshBtn);

    m_powerSwitch->setFixedHeight(36);
    m_powerSwitch->setContentsMargins(0, 0, 0, 0);
    m_discoverySwitch->leftWidget()->setMinimumWidth(10);
    m_discoverySwitch->setContentsMargins(0, 0, 0, 0);
    m_discoverySwitch->setFixedHeight(36);

    SettingsGroup *settingsGrp = new SettingsGroup(nullptr, SettingsGroup::GroupBackground);
    settingsGrp->setContentsMargins(0, 0, 0, 0);
    settingsGrp->layout()->setMargin(0);
    settingsGrp->setSpacing(1);

    settingsGrp->appendItem(m_powerSwitch);
    settingsGrp->appendItem(m_discoverySwitch);

    m_myDeviceListView = new DListView(this);
    m_myDeviceModel = new QStandardItemModel(m_myDeviceListView);
    m_myDeviceListView->setFrameShape(QFrame::NoFrame);
    m_myDeviceListView->setModel(m_myDeviceModel);
    m_myDeviceListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_myDeviceListView->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_myDeviceListView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_myDeviceListView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_myDeviceListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_myDeviceListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_myDeviceListView->setSelectionMode(QAbstractItemView::NoSelection);
    m_myDeviceListView->setViewportMargins(0, 0, 0, 0);

    m_otherDeviceListView = new DListView(this);
    m_otherDeviceModel = new QStandardItemModel(m_otherDeviceListView);
    m_otherDeviceListView->setFrameShape(QFrame::NoFrame);
    m_otherDeviceListView->setModel(m_otherDeviceModel);
    m_otherDeviceListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_otherDeviceListView->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_otherDeviceListView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_otherDeviceListView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_otherDeviceListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_otherDeviceListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_otherDeviceListView->setSelectionMode(QAbstractItemView::NoSelection);
    m_otherDeviceListView->setViewportMargins(0, 0, 0, 0);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(10);

    layout->addWidget(settingsGrp);
    layout->addSpacing(10);
    layout->addWidget(m_myDevicesGroup);
    layout->addWidget(m_myDeviceListView);
    layout->addSpacing(10);

    layout->addWidget(m_otherDevicesGroup);
    layout->addLayout(phlayoutShowAnonymous);
    layout->addWidget(m_otherDeviceListView);
    layout->addSpacing(30);
    layout->addStretch();
    layout->setContentsMargins(QMargins(10,0,10,10));

    setLayout(layout);
}

void AdapterWidget::initConnect()
{
    // 控制开关
    connect(m_powerSwitch, &SwitchWidget::checkedChanged, this, [ = ](const bool check){
        if (!check){
            onPowerStatus(false, false);
        }
        Q_EMIT requestSetToggleAdapter(m_adapter, check);
    });

    // 点击我的设备
    connect(m_myDeviceListView, &DListView::clicked, this, [this](const QModelIndex &idx){

    });

    // 点击其他设备
    connect(m_otherDeviceListView, &DListView::clicked, this, [=](const QModelIndex &idx){
        m_myDeviceListView->clearSelection();
        const QStandardItemModel *deviceModel = dynamic_cast<const QStandardItemModel *>(idx.model());
        if (!deviceModel) {
            return;
        }
        DStandardItem *item = dynamic_cast<DStandardItem *>(deviceModel->item(idx.row()));
        if (!item) {
            return;
        }
        for (auto it : m_deviceLists) {
            if (it && it->device() && it->getStandardItem() == item) {
                Q_EMIT requestConnectDevice(it->device(), m_adapter);
                break;
            }
        }
    });

    connect(m_refreshBtn, &DIconButton::clicked, this, [ = ] {
        Q_EMIT requestRefresh(m_adapter);
    });


}

void AdapterWidget::categoryDevice(DeviceSettingsItem *deviceItem, const bool paired)
{
    qDebug() << "device date move to SttingsItem !";
    if (deviceItem){
        if (paired) {
            // 此处已配对Item
            DStandardItem *dListItem = deviceItem->getStandardItem(m_myDeviceListView);
            m_myDevice << deviceItem;
            m_myDeviceModel->insertRow(0, dListItem);
        } else {
            DStandardItem *dListItem = deviceItem->getStandardItem(m_otherDeviceListView);
            if (m_showAnonymousCheckBox->checkState() == Qt::CheckState::Unchecked) {
                // 隐藏无名称的设备
                if (deviceItem->device() && !deviceItem->device()->name().isEmpty()) {
                    m_otherDeviceModel->insertRow(0, dListItem);
                }
            }else {
                m_otherDeviceModel->insertRow(0, dListItem);
            }
        }
    }
    qDebug() << " ======== 有误新增 new device " << m_myDevice.size();
    bool isVisible = !m_myDevice.isEmpty() && m_powerSwitch->checked();
    m_myDevicesGroup->setVisible(isVisible);
    m_myDeviceListView->setVisible(isVisible);
}

void AdapterWidget::addDevice(const Device *device)
{
    qDebug() << " 添加其他设备窗口的 设备: " << device->name() << " ID: " << device->id();
    if (!device) return;
    QPointer<DeviceSettingsItem> deviceItem = new DeviceSettingsItem(device, style());
    categoryDevice(deviceItem, device->paired());

    // 处理连接状态:
    connect(device, &Device::pairedChanged, this, [this, deviceItem](const  bool isPaired) {
        if (deviceItem && deviceItem->device()) {
            qDebug() << "paired :" << deviceItem->device()->name() << " 将在未连接设备清除!";
            DStandardItem *item = deviceItem->getStandardItem();
            QModelIndex otherInx = m_otherDeviceModel->indexFromItem(item);
            m_otherDeviceModel->removeRow(otherInx.row());
            deviceItem->resetDeviceItem();

            DStandardItem *dListItem = deviceItem->createStandardItem(m_myDeviceListView);
            m_myDevice << deviceItem;
            m_myDeviceModel->insertRow(0, dListItem);
        } else {
            qDebug() << "unpaired :" << deviceItem->device()->name();
            for (auto it : m_myDevice) {
                if (it && it->device() && it == deviceItem) {
                    m_myDevice.removeOne(it);
                    break;
                }
            }
            DStandardItem *item = deviceItem->getStandardItem();
            QModelIndex myDeviceIndex = m_myDeviceModel->indexFromItem(item);
            if (myDeviceIndex.isValid()) {
                m_myDeviceModel->removeRow(myDeviceIndex.row());
                DStandardItem *dListItem = deviceItem->createStandardItem(m_otherDeviceListView);
                m_otherDeviceModel->insertRow(0, dListItem);
            }
        }
    });

    connect(deviceItem, &DeviceSettingsItem::requestShowDetail, this, [this](const Device * device) {
        Q_EMIT requestShowDetail(m_adapter, device);
    });

    m_deviceLists << deviceItem;
}

void AdapterWidget::removeDevice(const QString &deviceId)
{
    qDebug() << "从窗口中移除设备!";
    bool isFind = false;
    for (auto dev : m_myDevice) {
        if (dev && dev->device() && dev->device()->id() == deviceId) {
            DStandardItem *item = dev->getStandardItem();
            QModelIndex myDevindex = m_myDeviceModel->indexFromItem(item);
            m_myDeviceModel->removeRow(myDevindex.row());
            m_myDevice.removeOne(dev);
            m_deviceLists.removeOne(dev);

            delete dev;
            dev = nullptr;
            Q_EMIT notifyRefreshDevice();
            isFind = true;
            break;
        }
    }
    if (!isFind) {
        for (auto it : m_deviceLists) {
            if (it && it->device() && it->device()->id() == deviceId) {
                DStandardItem *item = it->getStandardItem();
                QModelIndex otherDeviceIndex = m_otherDeviceModel->indexFromItem(item);
                m_otherDeviceModel->removeRow(otherDeviceIndex.row());
                m_deviceLists.removeOne(it);
                delete it;
                it = nullptr;
                Q_EMIT notifyRefreshDevice();
                break;
            }
        }
    }
    if (m_myDevice.isEmpty()) {
        m_myDevicesGroup->hide();
        m_myDeviceListView->hide();
    }
}

void AdapterWidget::onPowerStatus(bool bPower, bool bDiscovering)
{
    qDebug() << " TODO: 适配器开关状态 " << bPower << bDiscovering;
    m_powerSwitch->setEnabled(true);
    m_powerSwitch->setChecked(bPower);
    m_discoverySwitch->setEnabled(true);
    m_discoverySwitch->setVisible(bPower);
    m_discoverySwitch->setVisible(bPower);
    m_myDevicesGroup->setVisible(bPower && !m_myDevice.isEmpty());
    m_otherDevicesGroup->setVisible(bPower);
    m_showAnonymousCheckBox->setVisible(bPower);
    m_hideAnonymousLabel->setVisible(bPower);
    m_spinner->setVisible(bPower && bDiscovering);
    m_refreshBtn->setVisible(bPower && !bDiscovering);
    m_myDeviceListView->setVisible(bPower && !m_myDevice.isEmpty());
    m_otherDeviceListView->setVisible(bPower);

    Q_EMIT notifyRefreshDevice();
}
