#include "bluetoothworker.h"

namespace dcc {
namespace bluetooth {

BluetoothWorker &BluetoothWorker::Instance(bool sync)
{
    static BluetoothWorker worker(new BluetoothModel, sync);
    return worker;
}

BluetoothWorker::BluetoothWorker(BluetoothModel *model, bool sync)
    : QObject ()
    , m_bluetoothInter(new DBusBluetooth("com.deepin.daemon.Bluetooth", "/com/deepin/daemon/Bluetooth", QDBusConnection::sessionBus(), this))
    , m_model(model)
{
    connect(m_bluetoothInter, &DBusBluetooth::AdapterAdded, this, &BluetoothWorker::addAdapter);
    connect(m_bluetoothInter, &DBusBluetooth::AdapterRemoved, this, &BluetoothWorker::removeAdapter);
    connect(m_bluetoothInter, &DBusBluetooth::AdapterPropertiesChanged, this, &BluetoothWorker::onAdapterPropertiesChanged);
    connect(m_bluetoothInter, &DBusBluetooth::DeviceAdded, this, &BluetoothWorker::addDevice);
    connect(m_bluetoothInter, &DBusBluetooth::DeviceRemoved, this, &BluetoothWorker::removeDevice);
    connect(m_bluetoothInter, &DBusBluetooth::DevicePropertiesChanged, this, &BluetoothWorker::onDevicePropertiesChanged);

    m_bluetoothInter->setSync(sync);

    // 第一次初始化 需要同步调用 确保页面显示
    refresh(true);
    m_bluetoothInter->setSync(false);
}

void BluetoothWorker::onAdapterPropertiesChanged(const QString &json)
{
    qDebug() << "后端传递的适配器属性发生改变";
    const QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    const QJsonObject obj = doc.object();
    const QString id = obj["Path"].toString();

    Adapter *adapter = const_cast<Adapter*>(m_model->adapterById(id));
    if (adapter)
        inflateAdapter(adapter, obj);
}

void BluetoothWorker::onDevicePropertiesChanged(const QString &json)
{
    qDebug() << "后端传递的设备属性发生改变";
    const QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    const QJsonObject obj = doc.object();
    const QString id = obj["Path"].toString();
    const QString name = obj["Name"].toString();
    for (const Adapter *adapter : m_model->adapters()) {
        Adapter *adatetPointer = const_cast<Adapter *>(adapter);
        Device *device = const_cast<Device *>(adatetPointer->deviceById(id));
        if (device) {
            // 重新刷新 所有设备数据
            if (device->name() == name) {
                inflateDevice(device, obj);
            } else {
                if (!adatetPointer)
                    return;
                adatetPointer->removeDevice(device->id());
                inflateDevice(device, obj);
                adatetPointer->addDevice(device);
            }
        }
    }
}

void BluetoothWorker::addAdapter(const QString &json)
{
    qDebug() << "addAdapter";
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();

    Adapter *adapter = new Adapter();
    inflateAdapter(adapter,obj);
    m_model->addAdapter(adapter);
}

void BluetoothWorker::removeAdapter(const QString &json)
{
    qDebug() << "removeAdapter";
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();
    const QString id = obj["Path"].toString();

    const Adapter *result = m_model->removeAdapater(id);
    Adapter *adapter = const_cast<Adapter*>(result);
    if (adapter) {
        adapter->deleteLater();
        adapter = nullptr;
    }
}

void BluetoothWorker::addDevice(const QString &json)
{
    qDebug() << "add new Device";
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();
    const QString adapterId = obj["AdapterPath"].toString();
    const QString id = obj["Path"].toString();

    const Adapter *result = m_model->adapterById(adapterId);
    Adapter *adapter = const_cast<Adapter *>(result);
    if (adapter) {
        const Device *deviceResult = adapter->deviceById(id);
        Device *device = const_cast<Device*>(deviceResult);
        if (!device)
            device = new Device(adapter);
        inflateDevice(device, obj);
        adapter->addDevice(device);
    }
}

void BluetoothWorker::removeDevice(const QString &json)
{
    qDebug() << "removeDevice";
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();
    const QString adapterId = obj["AdapterPath"].toString();
    const QString id = obj["Path"].toString();

    const Adapter *result = m_model->adapterById(adapterId);
    Adapter *adapter = const_cast<Adapter*>(result);
    if (adapter) {
        adapter->removeDevice(id);
    }
}

void BluetoothWorker::setAdapterDiscovering(const QDBusObjectPath &path, bool enable)
{
    m_bluetoothInter->SetAdapterDiscovering(path, enable);
}

void BluetoothWorker::setAdapterPowered(const Adapter *adapter, const bool &powered)
{
    // 设置预设机制 500ms后状态强制恢复
    QTimer *timer = new QTimer;
//    timer->setSingleShot(true);
//    timer->setInterval(500);
//    connect(timer, &QTimer::timeout, [ = ](){
//        adapter->recoveryStatus();
//    });

    QDBusObjectPath path(adapter->id());
    if (!powered) {
        qDebug() << " 处理关闭状态: " ;
        QDBusPendingCall call = m_bluetoothInter->ClearUnpairedDevice();
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, [ = ]{
            if (!call.isError()) {
                QDBusPendingCall adapterPoweredOffCall = m_bluetoothInter->SetAdapterPowered(path, false);
                QDBusPendingCallWatcher *watchers = new QDBusPendingCallWatcher(adapterPoweredOffCall, this);
                connect(watchers, &QDBusPendingCallWatcher::finished, [this, adapterPoweredOffCall, adapter, powered, timer]{
                    if (adapterPoweredOffCall.isError()){
                        qDebug() << adapterPoweredOffCall.error().message();
                        adapter->poweredChanged(adapter->powered(), adapter->discovering());
                    }

                    connect(adapter, &Adapter::poweredChanged, [ = ](const bool &receivePowerd, const bool &discovering){
                        qDebug() << "WORKER POWER TEST : " << receivePowerd << powered << adapter->powered();
                        if(receivePowerd == powered)
                            adapter->loadStatus(adapter->powered());
                    });
                    delete timer;
                });
            } else {
                qDebug() << call.error().message();
            }
        });
    } else {
        qDebug() << "处理开启状态: ";
        QDBusPendingCall adapterPoweredOnCall  = m_bluetoothInter->SetAdapterPowered(path, true);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(adapterPoweredOnCall, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, [this, adapterPoweredOnCall, adapter, powered, timer] {
            if (adapterPoweredOnCall.isError()) {
                qDebug() << adapterPoweredOnCall.error().message();
                adapter->poweredChanged(adapter->powered(), adapter->discovering());
                return;
            }
            connect(adapter, &Adapter::poweredChanged, [ = ](const bool &receivePowerd, const bool &) {
                qDebug() << "WORKER POWER TEST : " << receivePowerd << powered << adapter->powered();
                if(receivePowerd == powered)
                    adapter->loadStatus(adapter->powered());
            });
            delete timer;
        });
    }
}

void BluetoothWorker::connectDevice(const Device *device, const Adapter *adapter)
{
    // 判断传入的设备是否存在
    for (const Adapter *ada : m_model->adapters()) {
        for (const Device *dev : ada->devices()) {
            Device *tmp = const_cast<Device *>(dev);
            if (tmp)
                tmp->setConnecting(dev == device);
        }
    }

    QDBusObjectPath path(device->id());
    m_bluetoothInter->ConnectDevice(path, QDBusObjectPath(adapter->id()));
    qDebug() << "DO => 连接完成 : " << device->name();
}

void BluetoothWorker::disconnectDevice(const Device *device)
{
    qDebug() << "TODO : disconnectDevice " << device->name();
    m_bluetoothInter->DisconnectDevice(QDBusObjectPath(device->id()));
}

void BluetoothWorker::ignoreDevice(const Adapter *adapter, const Device *device)
{
    qDebug() << "TODO : ignoreDevice 忽略" << device->name();
    m_bluetoothInter->RemoveDevice(QDBusObjectPath(adapter->id()), QDBusObjectPath(device->id()));
}

void BluetoothWorker::refresh(bool beFirst)
{
    qDebug() << "refresh";
    if (!m_bluetoothInter->isValid()) return;

    auto toCall = [this](const QDBusReply<QString> &req){
        const QString replyStr = req.value();
        qDebug() << "获取后端传递的 JSON : " << replyStr;
        QJsonDocument doc = QJsonDocument::fromJson(replyStr.toUtf8());
        QJsonArray arr = doc.array();
        for (QJsonValue val : arr) {
            Adapter *adapter = new Adapter(m_model);
            // 填充适配器
            inflateAdapter(adapter, val.toObject());

            m_model->addAdapter(adapter);
        }
    };

    if (beFirst) {
        QDBusInterface *inter = new QDBusInterface("com.deepin.daemon.Bluetooth",
                                                   "/com/deepin/daemon/Bluetooth",
                                                   "com.deepin.daemon.Bluetooth",
                                                   QDBusConnection::sessionBus());
        QDBusReply<QString> reply = inter->call("GetAdapters");
        toCall(reply);
    } else {
        // 若不是第一次 使用异步加载
        QDBusPendingCall call = m_bluetoothInter->GetAdapters();
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, [ = ] {
            if (!call.isError()) {
                QDBusReply<QString> reply = call.reply();
                toCall(reply);
            } else {
                qDebug() << call.error().message();
            }
        });
    }
}

// 解析加载获得的JSON数据
void BluetoothWorker::inflateAdapter(Adapter *adapter, const QJsonObject &adapterObj)
{
    qDebug() << "开始解析加载获得的JSON数据";
    const QString path = adapterObj["Path"].toString();
    const QString alias = adapterObj["Alias"].toString();
    const bool powered = adapterObj["Powered"].toBool();
    const bool discovering = adapterObj["Discovering"].toBool();  // 扫描中
    const bool discovered = adapterObj["Discoverable"].toBool();

//    adapter
    adapter->setDiscoverabled(discovered);
    adapter->setId(path);
    adapter->setName(alias);
    qDebug() << " ==== 开始解析的数据: " << powered << discovering;
    adapter->setPowered(powered, discovering);

    Q_EMIT deviceEnableChanged();
    QPointer<Adapter> adapterPointer(adapter);

    QDBusObjectPath dPath(path);
    QDBusPendingCall call = m_bluetoothInter->GetDevices(dPath);
    QDBusPendingCallWatcher *dWatcher = new QDBusPendingCallWatcher(call, this);
    connect(dWatcher, &QDBusPendingCallWatcher::finished, this, [this, adapterPointer, call] {
        qDebug() << " 开始获取单一设备信息! ";
        if (!adapterPointer)
            return;

        Adapter *adapter = adapterPointer.data();
        if (!call.isError()) {
            QStringList tmpList;

            QDBusReply<QString> reply = call.reply();
            const QString replyStr = reply.value();
            QJsonDocument doc = QJsonDocument::fromJson(replyStr.toUtf8());
            QJsonArray arr = doc.array();
            for (QJsonValue val : arr) {
                const QString id = val.toObject()["Path"].toString();
                const QString name = val.toObject()["Name"].toString();

                // 添加蓝牙设备
                const Device *result = adapter->deviceById(id);
                Device *device = const_cast<Device *>(result);
                if (!device){
                    device = new Device(adapter);
                } else {
                    if (device->name() != name) adapter->removeDevice(device->id());
                }
                inflateDevice(device, val.toObject());
                adapter->addDevice(device);

                tmpList << id;
            }
            // 剔除无效设备
            for (const Device *device : adapter->devices()) {
                if (!tmpList.contains(device->id())) {
                    adapter->removeDevice(device->id());

                    Device *target = const_cast<Device*>(device);
                    if (target) target->deleteLater();
                }
            }
        }
    });
}

// 填充蓝牙设备
void BluetoothWorker::inflateDevice(Device *device, const QJsonObject &deviceObj)
{
    qDebug() << "蓝牙设备JSON: " /*<< deviceObj*/;
    const QString id = deviceObj["Path"].toString();
    const QString addr = deviceObj["Address"].toString();
    const QString alias = deviceObj["Alias"].toString();
    const QString name = deviceObj["Name"].toString();
    const bool paired = deviceObj["Paired"].toBool();  // 配对
    const Device::State state = Device::State(deviceObj["State"].toInt());
    const bool connectState = deviceObj["ConnectState"].toBool();
    const QString icon = deviceObj["Icon"].toString();

    // 空设备
    if (alias.isEmpty() && name.isEmpty())
        return ;

    qDebug() << " BluetoothWorker::inflateDevice 可添加的蓝牙设备: " << name << " === " << alias;
    device->setId(id);
    device->setAddress(addr);
    device->setName(name);
    device->setAlias(alias);
    device->setPaired(paired);
    device->setState(state, connectState);
    device->setDeviceType(icon);
}

BluetoothWorker::~BluetoothWorker()
{

}


} // namespace bluetooth
} // namespace dcc
