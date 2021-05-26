#include "accountsdetailwidget.h"
#include "accountsmodule.h"
#include "accountswidget.h"
#include "createaccountpage.h"

#include "modules/accounts/usermodel.h"
#include "window/modules/accounts/accountsworker.h"

using namespace dcc::accounts;
using namespace DCC_NAMESPACE::accounts;

AccountsModule::AccountsModule(FrameProxyInterface *frame, QObject *parent)
    : QObject (parent)
    , ModuleInterface (frame)
{
    m_frameProxy = frame;
    m_pMainWindow = dynamic_cast<MainWindow *>(m_frameProxy);
}

void AccountsModule::initialize()
{
    if (m_userModel)
        delete m_userModel;

    m_userModel = new UserModel(this);
    m_accountsWorker = new AccountsWorker(m_userModel);

    m_accountsWorker->active();
    connect(m_accountsWorker, &AccountsWorker::requestMainWindowEnabled, this, &AccountsModule::onSetMainWindowEnabled);

}

const QString AccountsModule::name() const
{
    return QStringLiteral("accounts");
}

void AccountsModule::active()
{
    m_accountsWidget = new AccountsWidget;
    m_accountsWidget->setVisible(false);
    m_accountsWidget->setModel(m_userModel);
    connect(m_accountsWidget, &AccountsWidget::requestShowAccountsDetail, this, &AccountsModule::onShowAccountsDetailWidget);
    connect(m_accountsWidget, &AccountsWidget::requestCreateAccount, this, &AccountsModule::onShowCreateAccountPage);
    connect(m_accountsWidget, &AccountsWidget::requestBack, this, [ = ] {
        m_frameProxy->popWidget(this);
    });
    m_frameProxy->pushWidget(this, m_accountsWidget);
    m_accountsWidget->setVisible(true);
    // 显示默认用户
    m_accountsWidget->showDefaultAccountInfo();
}

AccountsModule::~AccountsModule()
{

}

void AccountsModule::onShowAccountsDetailWidget(dcc::accounts::User *account)
{
    AccountsDetailWidget *w = new AccountsDetailWidget(account);
    w->setVisible(true);

    connect(m_userModel, &UserModel::deleteUserSuccess, w, &AccountsDetailWidget::requestBack);
    connect(w, &AccountsDetailWidget::requestDeleteAccount, m_accountsWorker, &AccountsWorker::deleteAccount);
    connect(w, &AccountsDetailWidget::requestBack, this, [&]() {
        m_accountsWidget->setShowFirstUserInfo(false);
    });

    m_frameProxy->pushWidget(this, w);
}

// 创建用户
void AccountsModule::onShowCreateAccountPage()
{
    CreateAccountPage *w = new CreateAccountPage();
    w->setVisible(true);
    User *newUser = new User(this);
    w->setModel(m_userModel, newUser);
    connect(w, &CreateAccountPage::requestCreateUser, m_accountsWorker, &AccountsWorker::createAccount);
    connect(m_accountsWorker, &AccountsWorker::requestBack, m_accountsWidget, &AccountsWidget::toRequestBack);
    m_frameProxy->pushWidget(this, w);
}

void AccountsModule::onSetMainWindowEnabled(const bool isEnabled)
{
    if (m_pMainWindow)
        m_pMainWindow->setEnabled(isEnabled);
}
