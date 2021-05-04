#pragma once

#include "namespace.h"
#include <DFloatingButton>
#include <DListView>
#include <DStandardItem>

#include <QWidget>
#include <QGSettings>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QStandardItem;
class QStandardItemModel;
QT_END_NAMESPACE

namespace dcc {
namespace accounts {
class User;
class UserModel;
}
namespace widgets {
class MultiSelectListView;
}
}

namespace DCC_NAMESPACE {
namespace accounts {
class AccountsDetailWidget;
class MySortFilterProxyModel;
//显示用户列表
class AccountsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AccountsWidget(QWidget *parent = nullptr);
    virtual ~AccountsWidget();
    void setModel(dcc::accounts::UserModel *model);
    void showDefaultAccountInfo();
    void showLastAccountInfo();
    void setShowFirstUserInfo(bool show);
    void connectUserWithItem(dcc::accounts::User *user);

    enum AccountRole {
        ItemDataRole = Dtk::UserRole + 1
    };

    enum ActionOption {
        ClickCancel = 0,
        CreateUserSuccess,
        ModifyPwdSuccess
    };

    QPixmap pixmapToRound(const QPixmap &src);
    void handleRequestBack(AccountsWidget::ActionOption option = AccountsWidget::ClickCancel);



private:
    DTK_WIDGET_NAMESPACE::DFloatingButton *m_createBtn;
    dcc::widgets::MultiSelectListView *m_userlistView;
    QStandardItemModel *m_userItemModel;
    dcc::accounts::UserModel *m_userModel;
    QList<dcc::accounts::User *> m_userList;
    bool m_isShowFirstUserInfo = false;
    bool m_currentUserAdded = false;
    int m_saveClickedRow;
    QGSettings *m_accountSetting{nullptr};
    bool m_isCreateValid;
};

}   // namespace accounts
}   // namespace dccV20
