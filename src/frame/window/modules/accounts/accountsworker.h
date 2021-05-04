#ifndef ACCOUNTSWORKER_H
#define ACCOUNTSWORKER_H

#include <QObject>

#include <modules/accounts/usermodel.h>


namespace dcc {
namespace accounts {

class AccountsWorker : public QObject
{
    Q_OBJECT

public:
    explicit AccountsWorker(UserModel * userList, QObject *parent = 0);
    ~AccountsWorker();

private:
    UserModel *m_userModel;

};

}   // namespace accounts
}   // namespace dcc

#endif // ACCOUNTSWORKER_H
