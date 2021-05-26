#ifndef ADAPTER_H
#define ADAPTER_H

#include "device.h"
#include <QObject>
/**
 * @brief The Adapter 蓝牙适配器
 */
namespace dcc {
namespace bluetooth {

class Adapter : public QObject
{
    Q_OBJECT
public:
    explicit Adapter(QObject *parent = nullptr);

    inline QString name() const { return m_name; }
    void setName(const QString &name);

    inline QString id() const { return m_id; }
    void setId(const QString &id);

    QMap<QString, const Device *> devices() const;
    QList<QString> devicesId() const;
    const Device *deviceById(const QString &id) const;

    inline bool powered() const { return m_powered; }
    void setPowered(bool powered, bool discovering);

    inline bool discoverabled() const { return m_discoverable; }
    void setDiscoverabled(const bool discoverable);

    inline bool discovering() const { return m_discovering; }

public Q_SLOTS:
    void addDevice(const Device *device);
    void removeDevice(const QString &deviceId);

Q_SIGNALS:
    void discoverableChanged(const bool discoverable) const;
    void nameChanged(const QString &name) const;
    void poweredChanged(const bool &powered, const bool &discovering) const;
    void deviceRemoved(const QString &deviceId) const;
    void deviceAdded(const Device *device) const;

private:
    QString m_id;
    QString m_name;
    bool m_powered;
    bool m_discovering;
    bool m_discoverable;
    QMap<QString, const Device *> m_devices;
    QList<QString> m_devicesId; // 设备ID
};

} // namespace bluetooth
} // namespace dcc

#endif // ADAPTER_H
