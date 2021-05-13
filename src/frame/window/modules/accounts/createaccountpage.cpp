#include "createaccountpage.h"
#include "deepin_pw_check.h"

#include <widgets/titlelabel.h>

#include <DSuggestButton>
#include <QPushButton>
#include <DDesktopServices>
#include <QScroller>
#include <qdatetime.h>

DWIDGET_USE_NAMESPACE
using namespace dcc::accounts;
//using namespace dcc::widgets;
using namespace DCC_NAMESPACE::accounts;
using namespace DCC_NAMESPACE;

CreateAccountPage::CreateAccountPage(QWidget *parent)
    : QWidget (parent)
    , m_newUser{ nullptr }
    , m_nameEdit(new DLineEdit)
    , m_fullnameEdit(new DLineEdit)
    , m_passwdEdit(new DPasswordEdit)
    , m_repeatpasswdEdit(new DPasswordEdit)
{
    QVBoxLayout *mainContentLayout = new QVBoxLayout;
    mainContentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    setLayout(mainContentLayout);

    m_scrollArea = new QScrollArea;
    QScroller::grabGesture(m_scrollArea->viewport(), QScroller::LeftMouseButtonGesture);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameStyle(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setContentsMargins(0, 0, 0, 0);
    mainContentLayout->addWidget(m_scrollArea);

    m_tw = new QWidget(this);
    QVBoxLayout *contentLayout = new QVBoxLayout(m_tw);
    contentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    contentLayout->setSpacing(0);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    m_scrollArea->setWidget(m_tw);

    initWidgets(contentLayout);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setMargin(0);

    QPushButton *cancleBtn = new QPushButton(" 取消 ");
    DSuggestButton *addBtn = new DSuggestButton(" 创建 ");
    cancleBtn->setDefault(true);
    addBtn->setDefault(true);
    btnLayout->addWidget(cancleBtn);
    btnLayout->addWidget(addBtn);
    mainContentLayout->addSpacing(0);
    mainContentLayout->addLayout(btnLayout);

    connect(cancleBtn, &QPushButton::clicked, this, &CreateAccountPage::requestBack);
    connect(addBtn, &DSuggestButton::clicked, this, &CreateAccountPage::createUser);
}

CreateAccountPage::~CreateAccountPage()
{

}

void CreateAccountPage::setModel(UserModel *userModel, User *user)
{
    m_userModel = userModel;
    m_newUser = user;

}

// 检测创建名称
bool CreateAccountPage::checkName()
{
    const QString &userName = m_nameEdit->lineEdit()->text();
    // 限定长度
    if (userName.size() < 3 || userName.size() > 32) {
        m_nameEdit->setAlert(true);
        m_nameEdit->showAlertMessage(tr("Username must be between 3 and 32 characters"), m_nameEdit, 2000);
        return false;
    }
    // 限定大小写
    const QString compStr = QString("1234567890") + QString("abcdefghijklmnopqrstuvwxyz") + QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    if (!compStr.contains(userName.at(0))) {
        m_nameEdit->setAlert(true);
        m_nameEdit->showAlertMessage(tr("The first character must be a letter or number"), m_nameEdit, 2000);
        return false;
    }
    // 特殊符号
    if (userName.contains(QRegExp("^\\d+$"))) {
        m_nameEdit->setAlert(true);
        m_nameEdit->showAlertMessage(tr("Your username should not only have numbers"), m_nameEdit, 2000);
        return false;
    }

    // 是否重复组 getAllGroups()临时
    QList<QString> groupList = m_userModel->getAllGroup();
    qDebug() << " ---- groupList" << groupList.size();
    for (QString &group : groupList) {
//        qDebug() << "===" << group;
        if (userName == group) {
            m_nameEdit->setAlert(true);
            m_nameEdit->showAlertMessage(tr("The name already exists"), m_nameEdit, 2000);
            return false;
        }
    }

    if (m_nameEdit->isAlert()) {
        m_nameEdit->setAlert(false);
        m_nameEdit->hideAlertMessage();
    }

    return true;
}

bool CreateAccountPage::checkPassword(DPasswordEdit *edit)
{
    // 验证两次密码一致
    if (edit == m_repeatpasswdEdit) {
        if (m_passwdEdit->lineEdit()->text() != m_repeatpasswdEdit->lineEdit()->text()) {
            m_repeatpasswdEdit->setAlert(true);
            m_repeatpasswdEdit->showAlertMessage(" 验证两次密码不一致 ", this->parentWidget(), 2000);
            return false;
        }
    }
    // 身份密码验证: deepin_pw_check
    QString username = m_nameEdit->text();
    ERROR_TYPE error = deepin_pw_check(username.toLocal8Bit().data(), edit->lineEdit()->text().toLocal8Bit().data(), LEVEL_STRICT_CHECK, nullptr);
    QString str = err_to_string(error);
    qDebug() << "password Error: " << str;
    return (error == PW_ERROR_TYPE::PW_NO_ERR);
}

void CreateAccountPage::initWidgets(QVBoxLayout *layout)
{
    TitleLabel *titleLabel = new TitleLabel(" 创建用户 ");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->addWidget(titleLabel);

    QLabel *nameLabel = new QLabel("用户名 : ");
    layout->addWidget(nameLabel);
    layout->addWidget(m_nameEdit);
    layout->addSpacing(7);

    QLabel *passwdLabel = new QLabel("密码 : ");
    layout->addWidget(passwdLabel);
    layout->addWidget(m_passwdEdit);
    layout->addSpacing(7);

    QLabel *repeatpasswdLabel = new QLabel("确认密码 : ");
    layout->addWidget(repeatpasswdLabel);
    layout->addWidget(m_repeatpasswdEdit);
    layout->addSpacing(7);

    // 名称
    connect(m_nameEdit, &DLineEdit::textEdited, this, [=] (const QString &strText) {
        if (m_nameEdit->isAlert()) {
            m_nameEdit->hideAlertMessage();
            m_nameEdit->setAlert(false);
        }

        if (strText.isEmpty())
            return;

        QString strTemp;
        int idx;
        for (idx = 0; idx < strText.size(); ++idx) {
            if ((strText[idx] >= '0' && strText[idx] <= '9') ||
                (strText[idx] >= 'a' && strText[idx] <= 'z') ||
                (strText[idx] >= 'A' && strText[idx] <= 'Z') ||
                (strText[idx] == '-' || strText[idx] == '_')) {
                strTemp.append(strText[idx]);
            } else {
                // TODO?? 加声音特效 DDesktopServices libdtkwidget-dev
                DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
                break;
            }
        }
        m_nameEdit->lineEdit()->blockSignals(true);
        m_nameEdit->lineEdit()->setText(strTemp);
        m_nameEdit->lineEdit()->setCursorPosition(idx);
        m_nameEdit->lineEdit()->blockSignals(false);
    });
    connect(m_nameEdit, &DLineEdit::editingFinished, this, &CreateAccountPage::checkName);
}

void CreateAccountPage::createUser()
{
    //校验输入的用户名和密码
    if (!checkName() || !checkPassword(m_repeatpasswdEdit) || !checkPassword(m_passwdEdit)) {
        return;
    }

    // 随机生成用户图片
    qsrand(static_cast<uint>(QDateTime::currentDateTime().toTime_t()));
    auto index = qrand() % 14;
    QString path = QString("/var/lib/AccountsService/icons/%1.png").arg(index);
    qDebug() << "随机生成的user Avatar " << path;

    m_newUser->setCurrentAvatar(path);
    m_newUser->setName(m_nameEdit->lineEdit()->text().simplified());
    m_newUser->setFullname(m_nameEdit->lineEdit()->text());
    m_newUser->setPassword(m_passwdEdit->lineEdit()->text());
    m_newUser->setRepeatPassword(m_repeatpasswdEdit->lineEdit()->text());
    // TODO:用户组

    Q_EMIT requestCreateUser(m_newUser);
}
