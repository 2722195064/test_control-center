#ifndef NETWORKMODULEWIDGET_H
#define NETWORKMODULEWIDGET_H

#include "namespace.h"
#include <DListView>

QT_BEGIN_NAMESPACE
class QListView;
class QVBoxLayout;
class QStandardItemModel;
class QProcess;
class QGSettings;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace network {

class NetworkModuleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NetworkModuleWidget();
};

}
}


#endif // NETWORKMODULEWIDGET_H
