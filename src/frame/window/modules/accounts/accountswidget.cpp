
#include "accountswidget.h"
#include "widgets/multiselectlistview.h"

#include <QHBoxLayout>

DWIDGET_USE_NAMESPACE
using namespace dcc::accounts;
using namespace DCC_NAMESPACE::accounts;

AccountsWidget::AccountsWidget(QWidget *parent)
    : QWidget(parent)
    , m_createBtn(new DFloatingButton(DStyle::SP_IncreaseElement, this))
    , m_userlistView(new dcc::widgets::MultiSelectListView(this))
    , m_userItemModel(new QStandardItemModel(this))
{
    m_createBtn->setFixedSize(50, 50);
    m_createBtn->setVisible(true);

    QVBoxLayout *mainContentLayout = new QVBoxLayout();
    mainContentLayout->setMargin(0);
    mainContentLayout->addWidget(m_userlistView);
    mainContentLayout->addWidget(m_createBtn, 0, Qt::AlignHCenter);

    m_userlistView->setFrameShape(QFrame::NoFrame);
    m_userlistView->setViewportMargins(QMargins(10, 10, 10, 10));
    m_userlistView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_userlistView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_userlistView->setEditTriggers(QListView::NoEditTriggers);
    m_userlistView->setDragEnabled(false);
    m_userlistView->setIconSize(QSize(30, 30));
    m_userlistView->setLayoutDirection(Qt::LeftToRight);
    m_userlistView->setModel(m_userItemModel);

    setLayout(mainContentLayout);

}

AccountsWidget::~AccountsWidget()
{

}

void AccountsWidget::setModel(UserModel *model)
{
    m_userModel = model;
    m_createBtn->setVisible(m_userModel);
}
