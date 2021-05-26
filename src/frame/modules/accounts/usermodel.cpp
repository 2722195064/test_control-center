#include "usermodel.h"

#include <QDebug>

using namespace dcc::accounts;


UserModel::UserModel(QObject *parent)
    : QObject (parent)
{

}

UserModel::~UserModel()
{

}

void UserModel::addUser(const QString &id, User *user)
{
    Q_ASSERT(!m_userList.contains(id));

    m_userList[id] = user;
    // TODO: 添加管理员逻辑
//    if (user->userType() == User::Administrator) {
//        Q_EMIT adminCntChange(++m_adminCnt);
//    }
    Q_EMIT userAdded(user);
}

void UserModel::removeUser(const QString &id)
{
    Q_ASSERT(m_userList.contains(id));

    User *user = m_userList[id];
    m_userList.remove(id);

    Q_EMIT userRemoved(user);
}

bool UserModel::contains(const QString &id)
{
    qDebug() << " UserModel::contains " << m_userList.size();
    return m_userList.contains(id);
}

void UserModel::setAllGroup(const QStringList &groups)
{
    if (m_allGroups == groups) {
        return;
    }
    m_allGroups = groups;
    Q_EMIT allGroupsChange(groups);
}
