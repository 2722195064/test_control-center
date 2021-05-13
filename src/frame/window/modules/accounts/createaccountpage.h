#include <DLineEdit>
#include <QScrollArea>
#include <QWidget>
#include <deepin_pw_check.h>
#include <dpasswordedit.h>

#include <modules/accounts/usermodel.h>
#include "namespace.h"
#include "modules/accounts/user.h"

#pragma once

namespace DCC_NAMESPACE {
namespace accounts {

//创建账户页面
class CreateAccountPage : public QWidget
{
    Q_OBJECT

public:
    typedef PW_ERROR_TYPE ERROR_TYPE;

    enum PassWordType {
        NormalPassWord,
        IncludeBlankSymbol
    };

public:
    explicit CreateAccountPage(QWidget *parent = nullptr);
    ~CreateAccountPage();
    void setModel(dcc::accounts::UserModel *userModel, dcc::accounts::User *user);

Q_SIGNALS:
    void requestCreateUser(const dcc::accounts::User *user);
    void requestBack(bool doBack);

private:
    void initWidgets(QVBoxLayout *layout);
    void createUser();

private Q_SLOTS:
    bool checkName();
    bool checkPassword(DTK_WIDGET_NAMESPACE::DPasswordEdit *edit);

private:
    dcc::accounts::User *m_newUser;
    dcc::accounts::UserModel *m_userModel;
    DTK_WIDGET_NAMESPACE::DLineEdit *m_nameEdit;
    DTK_WIDGET_NAMESPACE::DLineEdit *m_fullnameEdit;
    DTK_WIDGET_NAMESPACE::DPasswordEdit *m_passwdEdit;
    DTK_WIDGET_NAMESPACE::DPasswordEdit *m_repeatpasswdEdit;

    QWidget *m_tw;
    QScrollArea *m_scrollArea;
};

}
}
