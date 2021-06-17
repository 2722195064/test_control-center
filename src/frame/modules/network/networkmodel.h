#ifndef NETWORKMODEL_H
#define NETWORKMODEL_H

#include <QObject>

namespace dcc {
namespace network {
class NetworkModel : public QObject
{
    Q_OBJECT
public:
    explicit NetworkModel(QObject *parent = nullptr);
    ~NetworkModel();
};

}
}

#endif // NETWORKMODEL_H
