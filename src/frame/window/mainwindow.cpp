#include "mainwindow.h"

#include <QHBoxLayout>
#include <DTitlebar>

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

}

void MainWindow::popWidget(ModuleInterface * const inter)
{

}
// 入口
void MainWindow::initAllmodule(const QString &m)
{

}

// 开启顶部视图 显示listview
void MainWindow::onFirstItemClick(const QModelIndex &index)
{

}
