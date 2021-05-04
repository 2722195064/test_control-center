#pragma once

#include "interfaces/moduleinterface.h"

namespace dcc {
namespace accounts {
class user;
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


private:
    dcc::accounts::UserModel *m_userModel{nullptr};
    dcc::accounts::AccountsWorker *m_accountsWorker{nullptr};
    AccountsWidget *m_accountsWidget = nullptr;

};

}   // namespace accounts
}   // namespace dccV20
