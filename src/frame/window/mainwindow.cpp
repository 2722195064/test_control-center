#include "mainwindow.h"
#include "interfaces/frameproxyinterface.h"
#include "modules/accounts/accountsmodule.h"
#include "modules/bluetooth/bluetoothmodule.h"

#include <QHBoxLayout>
#include <DTitlebar>
#include <QVariant>

using namespace DCC_NAMESPACE;
DTK_USE_NAMESPACE

//const QMargins navItemMargin(5, 3, 5, 3);
//const QVariant NavItemMargin = QVariant::fromValue(navItemMargin);

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow (parent)
    , m_contentLayout(nullptr)
    , m_rightContentLayout(nullptr)
    , m_navView(nullptr)
    , m_rightView(nullptr)
    , m_navModel(nullptr)
//    , m_bIsFinalWidget(false)
//    , m_bIsFromSecondAddWidget(false)
//    , m_topWidget(nullptr)
    , m_searchWidget(nullptr)
//    , m_firstCount(-1)
//    , m_widgetName("")
    , m_backwardBtn(nullptr)
{
    QWidget *content = new QWidget(this);
    content->setObjectName("contentwindow");
    m_contentLayout = new QHBoxLayout(content);
    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    m_contentLayout->setSpacing(0);
    m_rightContentLayout = new QHBoxLayout();
    m_rightContentLayout->setContentsMargins(0, 0, 0, 0);

    m_rightView = new DBackgroundGroup(m_rightContentLayout);
    m_rightView->setObjectName("modulepage");
    m_rightView->setItemSpacing(2);
    m_rightView->setContentsMargins(10, 10, 10, 10);

    m_navView = new DListView(this);
    m_navView->setFrameShape(QFrame::Shape::NoFrame);
    m_navView->setEditTriggers(QListView::NoEditTriggers);
    m_navView->setResizeMode(QListView::Adjust);
    m_navView->setAutoScroll(true);

    m_contentLayout->addWidget(m_navView, 1);
    m_contentLayout->addWidget(m_rightView, 5);

    m_rightView->hide();
    m_rightView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    layout()->setMargin(0);
    setCentralWidget(content);

    // 顶部视图
    m_navModel = new QStandardItemModel(m_navView);
    m_navView->setModel(m_navModel);
    connect(m_navView, &DListView::clicked, m_navView, &DListView::activated);
    connect(m_navView, &DListView::activated, this, &MainWindow::onFirstItemClick);

    m_searchWidget = new DSearchEdit(this);
    m_searchWidget->setMinimumSize(350, 36);
    // TODO: 搜索框弹出

    // 返回
    m_backwardBtn = new DIconButton(this);
    m_backwardBtn->setAccessibleName("backwardbtn");
    m_backwardBtn->setEnabled(false);
    m_backwardBtn->setIcon(QStyle::SP_ArrowBack);
    QWidget *backWidget = new QWidget();
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addSpacing(5);
    btnLayout->addWidget(m_backwardBtn);
    backWidget->setLayout(btnLayout);
    connect(m_backwardBtn, &DIconButton::clicked, this, [this] {
        qDebug() << "返回";
    });

    // 标题栏
    DTitlebar *titlebar = this->titlebar();

    titlebar->addWidget(m_searchWidget, Qt::AlignCenter);
    titlebar->addWidget(backWidget, Qt::AlignLeft | Qt::AlignVCenter);
    titlebar->setIcon(QIcon::fromTheme("preferences-system"));

    setMinimumSize(QSize(820, 634));
}

MainWindow::~MainWindow()
{

}

void MainWindow::pushWidget(ModuleInterface * const inter, QWidget * const w)
{
    if (!inter)  {
        qDebug() << Q_FUNC_INFO << " inter is nullptr";
        return;
    }

    if (!w)  {
        qDebug() << Q_FUNC_INFO << " widget is nullptr";
        return;
    }

    pushNormalWidget(inter, w);
}

void MainWindow::popWidget(ModuleInterface * const inter)
{

}

void MainWindow::setModuleVisible(ModuleInterface * const inter, const bool visible)
{
    qDebug() << inter->name() << " 页面隐藏 ";
    // TODO: 蓝牙页面隐藏
}

// 入口
void MainWindow::initAllmodule(const QString &m)
{
    // 避免重复加载
    if(m_existenceInit)
        return;
    m_existenceInit = true;

    using namespace accounts;
    using namespace bluetooth;
    // 实例化显示module
    m_modules = {
        {new AccountsModule(this), "帐户"},
        {new BluetoothModule(this), "蓝牙"},

    };
//     TODO:通过Gsetting 设置module是否可见

//  cbegin   Container<T>::const_iterator
    for (auto it = m_modules.cbegin(); it != m_modules.cend(); ++it){
        DStandardItem *item = new DStandardItem;
//        item->setIcon(it->first->icon()); 
        item->setText(it->second);
        item->setAccessibleText(it->second);
//        item->setData(NavItemMargin, Dtk::MarginsRole);

        m_navModel->appendRow(item);
    }

    // 设置模块大小
    bool isIcon = m_contentStack.empty();
    resetNavList(isIcon);

    // 模块初始化
    modulePreInitialize(m);
}

// 第一次点击顶部视图
void MainWindow::onFirstItemClick(const QModelIndex &index)
{
    if (!m_contentStack.isEmpty())
        return;

    ModuleInterface *inter = m_modules[index.row()].first;

    m_navView->setFocus();
    popAllWidgets();

    if (!m_initList.contains(inter)){
        inter->initialize();  // 开始初始化各模块
        m_initList << inter;
    }

    setCurrModule(inter);
    inter->active(); // 显示加载各二级菜单

    m_navView->clearSelection();
    m_navView->setSelectionMode(DListView::SingleSelection);
    m_navView->setCurrentIndex(index);
}

void MainWindow::popAllWidgets(int place)
{
    for (int pageCount = m_contentStack.count(); pageCount > place ; pageCount--) {
        popWidget();
    }
}

void MainWindow::popWidget()
{
    if (!m_contentStack.size()) return;

    // 显示原则: 先删除 后push
    QWidget *w = m_contentStack.pop().second;
    m_rightContentLayout->removeWidget(w);
    w->setParent(nullptr);
    w->deleteLater();
}

// 开启顶部视图 显示listview  此处显示控制中心两个视图模式
void MainWindow::resetNavList(bool isIconMode)
{
    if (isIconMode && m_navView->viewMode() == QListView::IconMode)
        return;

    if (!isIconMode && m_navView->viewMode() == QListView::ListMode)
        return;

    if (isIconMode) {
        // 显示顶部页面module
        m_navView->setViewportMargins(QMargins(0, 0, 0, 0));
        m_navView->setViewMode(QListView::IconMode);
        m_navView->setDragEnabled(false);
        m_navView->setMaximumWidth(QWIDGETSIZE_MAX);
        m_navView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_navView->setIconSize(QSize(84,84)); // 设置大小
        m_navView->setItemSize(QSize(170,168));
        m_navView->setItemSpacing(0);
        m_navView->setSpacing(20);
        m_navView->clearSelection();
        m_navView->setSelectionMode(QAbstractItemView::NoSelection);

        // 圆角弧度
        DStyle::setFrameRadius(m_navView, 18);
        m_rightView->hide();
        m_backwardBtn->setEnabled(false);
    } else {
        m_navView->setViewportMargins(QMargins(10, 10, 10, 10));
        m_navView->setViewMode(QListView::ListMode);
        m_navView->setMinimumWidth(188);
        m_navView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        m_navView->setIconSize(QSize(42,42));
        m_navView->setItemSize(QSize(168,48));
        m_navView->setSpacing(0);
        m_navView->setSelectionMode(QAbstractItemView::SingleSelection);

        DStyle::setFrameRadius(m_navView, 8);
        // 选中当前的项
        m_navView->selectionModel()->select(m_navView->currentIndex(), QItemSelectionModel::SelectCurrent);
        m_rightView->show();
        m_backwardBtn->setEnabled(true);
        m_contentStack.top().second->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    m_navView->setAutoFillBackground(!isIconMode);

}

void MainWindow::modulePreInitialize(const QString &m)
{
    for (auto it = m_modules.cbegin(); it != m_modules.cend(); ++it) {
        QElapsedTimer et;
        et.start();
        it->first->preInitialize(m == it->first->name());
        qDebug() << QString("initialize %1 module using time: %2ms")
                 .arg(it->first->name())
                 .arg(et.elapsed());
        // TODO: 设置模块是否可见 setModuleVisible
    }
}

void MainWindow::pushNormalWidget(ModuleInterface * const inter, QWidget * const w)
{
    // 1. 避免多次点击生成多个窗口
    popAllWidgets(1);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_contentStack.push( {inter, w} );
    qDebug() << "m_contentStack 当前stack size " << m_contentStack.size();
    m_rightContentLayout->addWidget(w);

    // 设置二级三级菜单宽度
    if (m_contentStack.size() == 2) {
        m_contentStack.at(0).second->setMinimumWidth(230);
        m_contentStack.at(1).second->setMinimumWidth(340);
    }
    resetNavList(m_contentStack.empty());
}
