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

    inline QList<User *> userList() const { return  m_userList.values(); }

    void addUser(const QString &id, User *user);
    void removeUser(const QString &id);
    bool contains(const QString &id);

    void setAllGroup(const QStringList &groups);
    inline QStringList getAllGroup() { return  m_allGroups; }

Q_SIGNALS:
    void userAdded(User *user);
    void userRemoved(User *user);
    void allGroupsChange(const QStringList &groups);

private:
    QMap<QString, User *> m_userList;
    QStringList m_allGroups;

};

} // namespace accounts
} // namespace dcc

#endif // USERMODEL_H
