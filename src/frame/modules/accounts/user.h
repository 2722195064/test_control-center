#ifndef USER_H
#define USER_H

#include <QObject>
#include <QSet>
#include <QString>

namespace dcc {
namespace accounts {

class User : public QObject
{
    Q_OBJECT

public:
    // 用户类型
    enum UserType {
        StandardUser = 0,
        Administrator,
        Customized
    };

    explicit User(QObject *parent = 0);
};

} // namespace accounts
} // namespace dcc

#endif // USER_H
