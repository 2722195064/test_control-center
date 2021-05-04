#include "accountsmodule.h"
#include "accountswidget.h"

#include "modules/accounts/usermodel.h"
#include "window/modules/accounts/accountsworker.h"

using namespace dcc::accounts;
using namespace DCC_NAMESPACE::accounts;

AccountsModule::AccountsModule(FrameProxyInterface *frame, QObject *parent)
    : QObject (parent)
    , ModuleInterface (frame)
{
    m_frameProxy = frame;
}

void AccountsModule::initialize()
{
    if (m_userModel)
        delete m_userModel;

    m_userModel = new UserModel(this);
    m_accountsWorker = new AccountsWorker(m_userModel);


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
}

AccountsModule::~AccountsModule()
{

}
