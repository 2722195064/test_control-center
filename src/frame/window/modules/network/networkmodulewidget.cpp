#include "networkmodulewidget.h"
#include "window/utils.h"
#include <widgets/multiselectlistview.h>
#include <QBoxLayout>

using namespace dcc::widgets;
using namespace dcc::network;
using namespace DCC_NAMESPACE::network;

NetworkModuleWidget::NetworkModuleWidget()
    : QWidget ()
    , m_lvnmpages(new dcc::widgets::MultiSelectListView(this))
    , m_modelpages(new QStandardItemModel(this))
{
    m_lvnmpages->setFrameShape(QFrame::NoFrame);
    m_lvnmpages->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_lvnmpages->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_lvnmpages->setModel(m_modelpages);
    m_lvnmpages->setViewportMargins(ScrollAreaMargins);
    m_lvnmpages->setIconSize(ListViweIconSize);

    m_centralLayout = new QVBoxLayout();
    setMinimumWidth(250);
    m_centralLayout->setMargin(0);
    setLayout(m_centralLayout);

    DStandardItem *pppit = new DStandardItem("DSL");
//    pppit->setData(QVariant::fromValue(DSLPage));
    pppit->setIcon(QIcon::fromTheme("dcc_dsl"));
    m_modelpages->appendRow(pppit);

    DStandardItem *vpnit = new DStandardItem(tr("VPN"));
//    vpnit->setData(QVariant::fromValue(VPNPage));
    vpnit->setIcon(QIcon::fromTheme("dcc_vpn"));
    m_modelpages->appendRow(vpnit);

    DStandardItem *prxyit = new DStandardItem(tr("System Proxy"));
//    prxyit->setData(QVariant::fromValue(SysProxyPage));
    prxyit->setIcon(QIcon::fromTheme("dcc_system_agent"));
    m_modelpages->appendRow(prxyit);

    DStandardItem *aprxit = new DStandardItem(tr("Application Proxy"));
//    aprxit->setData(QVariant::fromValue(AppProxyPage));
    aprxit->setIcon(QIcon::fromTheme("dcc_app_proxy"));
    m_modelpages->appendRow(aprxit);

    DStandardItem *infoit = new DStandardItem(tr("Network Details"));
//    infoit->setData(QVariant::fromValue(NetworkInfoPage));
    infoit->setIcon(QIcon::fromTheme("dcc_network"));
    m_modelpages->appendRow(infoit);
    m_centralLayout->addWidget(m_lvnmpages);
}

void NetworkModuleWidget::setModel(dcc::network::NetworkModel *model)
{

}
