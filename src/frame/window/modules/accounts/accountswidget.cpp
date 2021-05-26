
#include "accountswidget.h"
#include "widgets/multiselectlistview.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QUrl>

#include <modules/accounts/usermodel.h>

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

    connect(m_userlistView, &QListView::clicked, this, &AccountsWidget::onItemClicked);
    connect(m_userlistView, &DListView::activated, m_userlistView, &QListView::clicked);
    connect(m_createBtn, &QPushButton::clicked, this, &AccountsWidget::requestCreateAccount);
}

AccountsWidget::~AccountsWidget()
{

}

void AccountsWidget::setModel(UserModel *model)
{
    m_userModel = model;
    m_createBtn->setVisible(m_userModel);


    // 添加账户
    for (auto user : model->userList()) {
        addUser(user);
    }
    connect(model, &UserModel::userRemoved, this, &AccountsWidget::removeUser);

}

void AccountsWidget::showDefaultAccountInfo()
{
    if (m_userlistView->count() > 0) {
        QModelIndex qindex = m_userItemModel->index(0, 0);
        m_userlistView->setCurrentIndex(qindex);
        Q_EMIT m_userlistView->clicked(qindex);
    }
}

void AccountsWidget::showLastAccountInfo()
{
    if (m_userlistView->count() > 0) {
        int lastindex = m_userItemModel->rowCount() - 1;
        QModelIndex qindex = m_userItemModel->index(lastindex, 0);
        m_userlistView->setFocus();
        m_userlistView->setCurrentIndex(qindex);
        Q_EMIT m_userlistView->clicked(qindex);
    }
}

void AccountsWidget::setShowFirstUserInfo(bool show)
{
    m_isShowFirstUserInfo = show;
}

void AccountsWidget::connectUserWithItem(User *user)
{
    connect(user, &User::nameChanged, this, [ = ](const QString &) {
        int tindex = m_userList.indexOf(user);
        auto titem = m_userItemModel->item(tindex);
        if (!titem) {
            return;
        }
        titem->setText(user->displayName());
    });
    connect(user, &User::fullnameChanged, this, [ = ](const QString &) {
        int tindex = m_userList.indexOf(user);
        auto titem = m_userItemModel->item(tindex);
        if (titem) {
            titem->setText(user->displayName());
        }
    });
    connect(user, &User::currentAvatarChanged, this, [ = ](const QString & avatar) {
        qDebug() << "账户图片 changed path -> " << avatar;
        int tindex = m_userList.indexOf(user);
        auto titem = m_userItemModel->item(tindex);
        if (!titem) {
            return;
        }
        auto ratio = this->devicePixelRatioF();
        auto path = avatar;
        if (ratio > 4.0) {
            path.replace("icons/", "icons/bigger/");
        }

        path = path.startsWith("file://") ? QUrl(path).toLocalFile() : path;
        QPixmap pixmap = pixmapToRound(path);

        titem->setIcon(QIcon(pixmap));
    });
}

QPixmap AccountsWidget::pixmapToRound(const QPixmap &src)
{
    if (src.isNull()) {
        return QPixmap();
    }

    auto pixmap = QPixmap(src);
    QSize size = pixmap.size();
    QPixmap mask(size);
    mask.fill(Qt::transparent);

    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addEllipse(0, 0, size.width(), size.height());
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, pixmap);

    return mask;
}

// 处理解决
void AccountsWidget::handleRequestBack(AccountsWidget::ActionOption option)
{
    switch (option) {
    case AccountsWidget::ClickCancel: { //点击取消
        QModelIndex qindex0 = m_userItemModel->index(m_saveClickedRow, 0);
        m_userlistView->setFocus();
        m_userlistView->setCurrentIndex(qindex0);
        onItemClicked(qindex0);
        }
        break;
    case AccountsWidget::CreateUserSuccess: { //创建账户成功
        QModelIndex qindex1 = m_userItemModel->index(m_userItemModel->rowCount() - 1, 0);
        m_userlistView->setFocus();
        m_userlistView->setCurrentIndex(qindex1);
        onItemClicked(qindex1);
        }
        break;
    case AccountsWidget::ModifyPwdSuccess: { //修改密码成功
        QModelIndex qindex2 = m_userItemModel->index(0, 0);
        m_userlistView->setFocus();
        m_userlistView->setCurrentIndex(qindex2);
        onItemClicked(qindex2);
        }
        break;
    }
}

void AccountsWidget::toRequestBack(bool toBack)
{
    if(toBack) {
        QModelIndex qindex1 = m_userItemModel->index(m_userItemModel->rowCount() - 1, 0);
        m_userlistView->setFocus();
        m_userlistView->setCurrentIndex(qindex1);
        onItemClicked(qindex1);
    }
}


void AccountsWidget::addUser(User *user, bool t1)
{
    m_userList << user;
    DStandardItem *item = new  DStandardItem;
    item->setData(0);

    // TODO: 需要区分用户版本
    // TODO: 添加当前联机用户

    m_userItemModel->appendRow(item);
    connectUserWithItem(user);

}

void AccountsWidget::removeUser(User *user)
{
    m_userItemModel->removeRow(m_userList.indexOf(user)); // It will delete when remove
    m_userList.removeOne(user);

    if (m_userList.isEmpty()) {
        Q_EMIT requestBack();
        return;
    }

    m_isShowFirstUserInfo ? showDefaultAccountInfo() : showLastAccountInfo();
}

void AccountsWidget::onItemClicked(const QModelIndex &index)
{
    qDebug() << "change User ! ";
    Q_EMIT requestShowAccountsDetail(m_userList[index.row()]);
    m_userlistView->resetStatus(index);
}
