
#pragma once
#include "namespace.h"
#include "modules/accounts/user.h"
#include "modules/accounts/usermodel.h"

#include <QLabel>
#include <QWidget>

#include <widgets/switchwidget.h>

QT_BEGIN_NAMESPACE
class QWidget;
class QScrollArea;
class QVBoxLayout;
class QPushButton;
class QSwitchButton;
QT_END_NAMESPACE

namespace dcc {
namespace accounts {
class UserModel;
}
}

namespace DCC_NAMESPACE {
namespace accounts {
class AccountsDetailWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AccountsDetailWidget(dcc::accounts::User *user, QWidget *parent = nullptr);
    virtual ~AccountsDetailWidget();
    void setAccountModel(dcc::accounts::UserModel *model);


Q_SIGNALS:
    void requestShowPwdSettings(dcc::accounts::User *user);
    void requestSetAutoLogin(dcc::accounts::User *user, const bool autoLogin);

protected:
    void initUserInfo(QVBoxLayout *layout);
    void initSetting(QVBoxLayout *layout);

private Q_SLOTS:


private:
    dcc::accounts::User *m_curUser;
    dcc::accounts::UserModel *m_userModel;

    QScrollArea *m_scrollArea;

    QLabel *m_fullName;//账户全名
    QPushButton *m_modifyPassword;
    QPushButton *m_deleteAccount;
//    QSwitchButton *m_autoLogin;
//    QSwitchButton *m_nopasswdLogin;

    dcc::widgets::SwitchWidget *m_autoLogin;
    dcc::widgets::SwitchWidget *m_nopasswdLogin;

};

}   // namespace accounts
}   // namespace dccV20
