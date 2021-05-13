#ifndef ACCOUNTSWORKER_H
#define ACCOUNTSWORKER_H

#include <QObject>
#include <modules/accounts/creationresult.h>
#include <modules/accounts/usermodel.h>

#include <com_deepin_daemon_accounts.h>
#include <com_deepin_daemon_accounts_user.h>

#include <org_freedesktop_displaymanager.h>

using Accounts = com::deepin::daemon::Accounts;
using AccountsUser = com::deepin::daemon::accounts::User;
using DisplayManager = org::freedesktop::DisplayManager;

namespace dcc {
namespace accounts {
class User;
class AccountsWorker : public QObject
{
    Q_OBJECT

public:
    explicit AccountsWorker(UserModel * userList, QObject *parent = 0);
    ~AccountsWorker();

    void active();

public:
    void addUser(const QString &userPath);
    void removeUser(const QString &userPath);
    void onUserListChanged(const QStringList &userList);

private:
    CreationResult *createAccountInternal(const User *user);
    // 盐值加密
    QString cryptUserPassword(const QString &password);

Q_SIGNALS:
    void requestMainWindowEnabled(const bool isEnabled) const;
    void requestBack(bool toBack);

public Q_SLOTS:
    void createAccount(const User *user);
    void getAllGroups();
    void getAllGroupsResult(QDBusPendingCallWatcher *watch);


private:
    Accounts *m_accountsInter;
    DisplayManager *m_dmInter; // 设置联机状态

    UserModel *m_userModel;
    QMap<User *, AccountsUser *> m_userInters;

    QString m_currentUserName;

};

}   // namespace accounts
}   // namespace dcc

#endif // ACCOUNTSWORKER_H
