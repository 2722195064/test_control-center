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
    , m_powerSwitch(new SwitchWidget(nullptr, m_titleEdit))
    , m_showAnonymousCheckBox(new DCheckBox(this))
    , m_discoverySwitch(new SwitchWidget("允许其他蓝牙设备找到此设备"))
//    , m_lastPowerCheck(false)
    , m_model(model)
    , m_bluetoothInter("com.deepin.daemon.Bluetooth", "/com/deepin/daemon/Bluetooth", QDBusConnection::sessionBus(), this)
{
    initMember();
    initUI();
    initConnect();
    // 设置对应适配器信息
    setAdapter(m_adapter);
}

AdapterWidget::~AdapterWidget()
{

}

void AdapterWidget::setAdapter(const Adapter *adapter)
{
    qDebug() << " ----- AdapterWidget::setAdapter  调整适配器的基本显示: ";

    m_titleEdit->setTitle(adapter->name());
    for (QString id : adapter->devicesId()) {
        if (adapter->devices().contains(id)) {
            const Device *device = adapter->devices()[id];
            addDevice(device);
        }
    }

    m_lastPowerCheck = adapter->powered();
    m_discoverySwitch->setChecked(m_adapter->discoverabled());
    onPowerStatus(m_lastPowerCheck,adapter->discovering());
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

}

void AdapterWidget::categoryDevice(DeviceSettingsItem *deviceItem, const bool paired)
{
    qDebug() << "device date move to SttingsItem !";
    if (deviceItem){
        if (paired) {
            // 此处已配对Item
            DStandardItem *dListItem = deviceItem->getStandardItem(m_myDeviceListView);
            m_myDevice << deviceItem;
            m_myDeviceModel->insertRow(0,dListItem);
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
    bool isVisible = !m_myDevice.isEmpty() && m_powerSwitch->checked();
    m_myDevicesGroup->setVisible(isVisible);
    m_myDeviceListView->setVisible(isVisible);
}

void AdapterWidget::addDevice(const Device *device)
{
    qDebug() << "添加适配器中的 设备: " << device->name() << " ID: " << device->id();
    if (!device) return;
    QPointer<DeviceSettingsItem> deviceItem = new DeviceSettingsItem(device, style());
    categoryDevice(deviceItem, device->paired());

    m_deviceLists << deviceItem;
}

void AdapterWidget::onPowerStatus(bool bPower, bool bDiscovering)
{
    qDebug() << "TODO: 适配器开关状态";
}
