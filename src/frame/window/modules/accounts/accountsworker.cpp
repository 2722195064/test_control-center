#include "accountsworker.h"

#include <QtConcurrent/QtConcurrent>
#include <QStandardPaths>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QFutureWatcher>

#include <pwd.h>
#include <unistd.h>
#include <libintl.h>
#include <random>
#include <crypt.h>
#include <polkit-qt5-1/PolkitQt1/Authority>

using namespace PolkitQt1;
using namespace dcc::accounts;

const QString AccountsService("com.deepin.daemon.Accounts");

AccountsWorker::AccountsWorker(UserModel *userList, QObject *parent)
    : QObject (parent)
    , m_accountsInter(new Accounts(AccountsService, "/com/deepin/daemon/Accounts", QDBusConnection::systemBus(), this))
    , m_userModel(userList)
{
    // /usr/include
    struct passwd *pws;
    pws = getpwuid(getuid());  // 通过当前进程ID  获取返回 passwd当前用户数据库信息
    m_currentUserName = QString(pws->pw_name);

    connect(m_accountsInter, &Accounts::UserListChanged, this, &AccountsWorker::onUserListChanged, Qt::QueuedConnection);
    connect(m_accountsInter, &Accounts::UserAdded, this, &AccountsWorker::addUser, Qt::QueuedConnection);
    connect(m_accountsInter, &Accounts::UserDeleted, this, &AccountsWorker::removeUser, Qt::QueuedConnection);

    // 加载后端信息
    QDBusInterface interface(AccountsService, "/com/deepin/daemon/Accounts", AccountsService, QDBusConnection::systemBus());
    onUserListChanged(interface.property("UserList").toStringList());

    getAllGroups();
}

AccountsWorker::~AccountsWorker()
{

}

void AccountsWorker::active()
{
    for (auto it(m_userInters.cbegin()); it != m_userInters.cend(); ++it) {
        it.key()->setName(it.value()->userName());
        it.key()->setAutoLogin(it.value()->automaticLogin());
        it.key()->setAvatars(it.value()->iconList());
        it.key()->setGroups(it.value()->groups());
        it.key()->setCurrentAvatar(it.value()->iconFile()); // 设置图像
        it.key()->setCreatedTime(it.value()->createdTime());
    }
}

void AccountsWorker::addUser(const QString &userPath)
{
    if (userPath.contains("User0", Qt::CaseInsensitive))
        return;

    AccountsUser *userInter = new AccountsUser(AccountsService, userPath, QDBusConnection::systemBus(), this);
    userInter->setSync(false);

    User *user = new User(this);
    connect(userInter, &AccountsUser::UserNameChanged, user, [=](const QString &name) {
        user->setName(name);
//        user->setOnline(m_onlineUsers.contains(name));
        user->setIsCurrentUser(name == m_currentUserName);

    });

    connect(userInter, &AccountsUser::AutomaticLoginChanged, user, &User::setAutoLogin);
    connect(userInter, &AccountsUser::IconListChanged, user, &User::setAvatars);
    connect(userInter, &AccountsUser::IconFileChanged, user, &User::setCurrentAvatar);
    connect(userInter, &AccountsUser::FullNameChanged, user, &User::setFullname);
    connect(userInter, &AccountsUser::NoPasswdLoginChanged, user, &User::setNopasswdLogin);
    connect(userInter, &AccountsUser::PasswordStatusChanged, user, &User::setPasswordStatus);
    connect(userInter, &AccountsUser::CreatedTimeChanged, user, &User::setCreatedTime);
    connect(userInter, &AccountsUser::GroupsChanged, user, &User::setGroups);
    connect(userInter, &AccountsUser::AccountTypeChanged, user, &User::setUserType);
    connect(userInter, &AccountsUser::MaxPasswordAgeChanged, user, &User::setPasswordAge);

    // 异步调用 change
    userInter->userName();
    userInter->fullName();
    userInter->automaticLogin();
    userInter->iconList();
    userInter->groups();
    userInter->iconFile();
    userInter->noPasswdLogin();
    userInter->passwordStatus();
    userInter->createdTime();
    userInter->accountType();
    userInter->maxPasswordAge();
    userInter->IsPasswordExpired();

    m_userInters[user] = userInter;
    m_userModel->addUser(userPath, user);
}

void AccountsWorker::removeUser(const QString &userPath)
{
    for (AccountsUser *userInter : m_userInters.values()) {
        if (userInter->path() == userPath) {
            User *user = m_userInters.key(userInter);
            user->deleteLater();

            m_userInters.remove(user);
            m_userModel->removeUser(userPath);

            return;
        }
    }
}

void AccountsWorker::onUserListChanged(const QStringList &userList)
{
    int count = 0;
    static bool first = true;
    for (const QString &path : userList) {
        if (!m_userModel->contains(path)) {
            count++;
            addUser(path);
            if (count > 50 && first) {
                first = false;
                break; // 第一次打开用户列表只加载51个用户信息
            } else if (count > 2 && !first) {
                break; // 滚动条每滑动一次，加载3个用户
            }
        }
    }
}

CreationResult *AccountsWorker::createAccountInternal(const User *user)
{
    CreationResult *result = new CreationResult;

    //接口验证
    QDBusPendingReply<bool, QString, int> reply = m_accountsInter->IsUsernameValid(user->name());
    reply.waitForFinished();
    if(reply.isError()){
        result->setType(CreationResult::UserNameError);
        result->setMessage(reply.error().message());

        return result;
    }
    bool validation = reply.argumentAt(0).toBool();
    if (!validation) {
        result->setType(CreationResult::UserNameError);
        // dgettext  <libintl.h>
        result->setMessage(dgettext("dde-daemon", reply.argumentAt(1).toString().toUtf8().data()));
        return result;
    }

    // validate password
    if (user->password() != user->repeatPassword()) {
        result->setType(CreationResult::PasswordMatchError);
        result->setMessage(tr("Password not match"));
        return result;
    }

    // 授权认证
    Authority::Result authenticationResult;
    // 弹出授权认证框
    authenticationResult = Authority::instance()->checkAuthorizationSync("com.deepin.daemon.accounts.user-administration", UnixProcessSubject(getpid()),
                                                                         Authority::AllowUserInteraction);
    if (Authority::Result::Yes != authenticationResult){
        result->setType(CreationResult::Canceled);
        return result;
    }

    QDBusObjectPath path;
    QDBusPendingReply<QDBusObjectPath> createReply = m_accountsInter->CreateUser(user->name(), user->fullname(), user->userType());
    createReply.waitForFinished();
    if(createReply.isError()) {
        result->setType(createReply.error().message().isEmpty() ? CreationResult::Canceled : CreationResult::UnknownError);
        result->setMessage(createReply.error().message());
        return result;
    }else {
        path = createReply.argumentAt<0>();
    }
    const QString userpath = path.path();
    qDebug() << "create user path " << userpath;

    AccountsUser *userDBus = new AccountsUser("com.deepin.daemon.Accounts", userpath, QDBusConnection::systemBus(), this);
    if (!userDBus->isValid()) {
        result->setType(CreationResult::UnknownError);
        result->setMessage("user dbus is still not valid.");

        return result;
    }

    // 自验证 密码加密
    bool sifResult = !userDBus->SetIconFile(user->currentAvatar()).isError();
    bool spResult = !userDBus->SetPassword(cryptUserPassword(user->password())).isError(); // 对用户密码进行加密
    bool groupResult = true;

    if (!sifResult || !spResult || !groupResult) {
        result->setType(CreationResult::UnknownError);
        if (!sifResult)
            result->setMessage("set icon file for new created user failed.");
        if (!spResult)
            result->setMessage("set password for new created user failed");
        if (!groupResult)
            result->setMessage("set group for new created user failed");
        return result;
    }

    return result;
}

//密码是salt和crypt函数的组合。 slat以$6$开始，16字节的随机值，在$结尾。 crypt函数将返回加密值。
QString AccountsWorker::cryptUserPassword(const QString &password)
{
    const QString seedchars("./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    char salt[] = "$6$................$";

    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(0, seedchars.size() - 1); //seedchars.size()是64，生成随机数的范围应该写成[0, 63]。

    // Random access to a character in a restricted list
    for (int i = 0; i != 16; i++) {
        salt[3 + i] = seedchars.at(uniform_dist(e1)).toLatin1();
    }

    QString saltpass = crypt(password.toUtf8().data(), salt);
    qDebug() << " --- 加密pass " << saltpass;
    return saltpass;
}

void AccountsWorker::createAccount(const User *user)
{
    qDebug() << "--- 开始创建用户 --- ";
    QFutureWatcher<CreationResult *> *watcher = new QFutureWatcher<CreationResult *>(this);
    connect(watcher, &QFutureWatcher<CreationResult *>::finished, [this, watcher]{
        qDebug() << " -- Futur -- ";
        CreationResult *result = watcher->result();
        m_userModel->setAllGroup(m_accountsInter->GetGroups());

        // TODO: 对result 的返回类型进行判断
//        if (result->type() == CreationResult::NoError)
        Q_EMIT requestBack(true);
        Q_EMIT requestMainWindowEnabled(true);
        watcher->deleteLater();
    });
    // TODO?? QFuture
    QFuture<CreationResult *> future = QtConcurrent::run(this, &AccountsWorker::createAccountInternal, user);
    watcher->setFuture(future);
//    createAccountInternal(user);
}

void AccountsWorker::getAllGroups()
{
    // 等待异步相应 通过finished 消息到达后发出
    QDBusPendingReply<QStringList> reply = m_accountsInter->GetGroups();
    QDBusPendingCallWatcher *groupResult = new  QDBusPendingCallWatcher(reply, this);
    connect(groupResult, &QDBusPendingCallWatcher::finished, this, &AccountsWorker::getAllGroupsResult);
}

void AccountsWorker::getAllGroupsResult(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QStringList> reply = *watch;
    if (!watch->isError()) {
        m_userModel->setAllGroup(reply.value());
    } else {
        qDebug() << "getAllGroupsResult error." << watch->error();
    }
    watch->deleteLater();
}

// 删除用户
void AccountsWorker::deleteAccount(User *user, const bool deleteHome)
{
    // 调用后端接口
    QDBusPendingReply<> reply = m_accountsInter->DeleteUser(user->name(), deleteHome);
    reply.waitForFinished();
    // 删除后操作
    Q_EMIT m_userModel->deleteUserSuccess();
    removeUser(m_userInters.value(user)->path());
    getAllGroups();
}
