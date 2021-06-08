#include "bluetoothmodel.h"
#include <QDebug>

namespace dcc {
namespace bluetooth {

BluetoothModel::BluetoothModel(QObject *parent)
    :QObject (parent)
{
    m_adapters.clear();
}

QMap<QString, const Adapter *> BluetoothModel::adapters() const
{
    qDebug() << " BluetoothModel::adapters() 获取最新size " << m_adapters.size();
    return m_adapters;
}

BluetoothModel::~BluetoothModel()
{

}

void BluetoothModel::addAdapter(Adapter *adapter)
{
    qDebug() << " 新增加 Adapter 开始调用后端接口";
    if (!adapterById(adapter->id())) {
        m_adapters[adapter->id()] = adapter;
        Q_EMIT adapterAdded(adapter);
        Q_EMIT adpaterListChanged();
        return;
    }
    adapter->deleteLater();
}

const Adapter *BluetoothModel::removeAdapater(const QString &adapterId)
{
    const Adapter *adapter = nullptr;
    adapter = adapterById(adapterId);
    if (adapter) {
        m_adapters.remove(adapterId);
        Q_EMIT adapterRemove(adapter);
        Q_EMIT adpaterListChanged();
    }
    return adapter;
}

const Adapter *BluetoothModel::adapterById(const QString &id)
{
//    if (m_adapters.keys().contains(id)) {
//        return m_adapters[id];
//    }else {
//        return  nullptr;
//    }

    return  m_adapters.keys().contains(id) ? m_adapters[id] : nullptr;
}


}
}
