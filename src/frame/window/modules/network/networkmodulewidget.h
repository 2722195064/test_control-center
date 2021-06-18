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

namespace dcc {
namespace network {
class NetworkModel;
}
}

namespace dcc {
namespace widgets {
class MultiSelectListView;
}
}

namespace DCC_NAMESPACE {
namespace network {

class NetworkModuleWidget : public QWidget
{
    Q_OBJECT
public:
    // 区分网络页面类型
    enum PageType {
        NonePage = -1,
        WiredPage = 0,
        WirelessPage,
        DSLPage,
        VPNPage,
        SysProxyPage,
        AppProxyPage,
        HotspotPage,
        NetworkInfoPage
    };

    explicit NetworkModuleWidget();
    void setModel(dcc::network::NetworkModel *model);

private:
    QVBoxLayout *m_centralLayout;
    dcc::widgets::MultiSelectListView *m_lvnmpages;
    QStandardItemModel *m_modelpages;
//    enum{
//        SectionRole = Dtk::UserRole + 1,
//        DeviceRole,
//        SearchPath
//    };

};

}
}


#endif // NETWORKMODULEWIDGET_H
