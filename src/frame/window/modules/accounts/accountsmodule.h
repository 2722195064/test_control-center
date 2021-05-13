#pragma once

#include "interfaces/moduleinterface.h"

#include <window/mainwindow.h>

namespace dcc {
namespace accounts {
class User;
class UserModel;
class AccountsWorker;
class AccountsWidget;
}
}

namespace DCC_NAMESPACE {
namespace accounts {
class AccountsWidget;
class AccountsModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit AccountsModule(FrameProxyInterface *frame, QObject *parent = nullptr);

    void initialize() override;
    const QString name() const override;
    void active() override;


private:
    ~AccountsModule();

public Q_SLOTS:
    void onShowAccountsDetailWidget(dcc::accounts::User *account);
    void onShowCreateAccountPage();
    void onSetMainWindowEnabled(const bool isEnabled);

private:
    dcc::accounts::UserModel *m_userModel{nullptr};
    dcc::accounts::AccountsWorker *m_accountsWorker{nullptr};
    AccountsWidget *m_accountsWidget = nullptr;
    MainWindow *m_pMainWindow = nullptr;

};

}   // namespace accounts
}   // namespace dccV20
