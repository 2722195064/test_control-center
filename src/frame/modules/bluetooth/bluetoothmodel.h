#ifndef BLUETOOTHMODEL_H
#define BLUETOOTHMODEL_H

#include "adapter.h"
#include <QObject>

namespace dcc {
namespace bluetooth {

class BluetoothModel : public QObject
{
    Q_OBJECT
public:
    explicit BluetoothModel(QObject *parent = nullptr);
    QMap<QString, const Adapter *> adapters() const;

    ~BluetoothModel();

public Q_SLOTS:
   // 新增蓝牙适配器
   void addAdapter(Adapter *adapter);
   const Adapter *removeAdapater(const QString &adapterId);
   const Adapter *adapterById(const QString &id);

Q_SIGNALS:
    void adpaterListChanged();
    void adapterAdded(const Adapter *adapter) const;
    void adapterRemove(const Adapter *adapter) const;

private:
    QMap<QString, const Adapter *> m_adapters;
};

} // namespace bluetooth
} // namespace dcc


#endif // BLUETOOTHMODEL_H
