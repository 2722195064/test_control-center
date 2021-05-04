#ifndef USERMODEL_H
#define USERMODEL_H

#include <QObject>
#include <QMap>

#include "user.h"

namespace dcc {
namespace accounts {

class UserModel : public QObject
{
    Q_OBJECT
public:
    explicit UserModel(QObject *parent = 0);
    ~UserModel();

private:
    QMap<QString, User *> m_userList;

};

} // namespace accounts
} // namespace dcc

#endif // USERMODEL_H
