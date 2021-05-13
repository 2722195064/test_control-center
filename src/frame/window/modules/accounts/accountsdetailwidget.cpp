#include "accountsdetailwidget.h"

#include <QBoxLayout>
#include <qscrollarea.h>

#include <modules/accounts/avatarwidget.h>

#include <widgets/settingsgroup.h>

DWIDGET_USE_NAMESPACE
using namespace dcc::accounts;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::accounts;

AccountsDetailWidget::AccountsDetailWidget(dcc::accounts::User *user, QWidget *parent)
    : QWidget(parent)
    , m_curUser(user)
    , m_scrollArea(new QScrollArea)
    , m_modifyPassword(new QPushButton(" 修改密码 ",this))
    , m_deleteAccount(new QPushButton(" 删除用户 ",this))
{
    //整体布局
    QVBoxLayout *mainContentLayout = new QVBoxLayout;
    mainContentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    setLayout(mainContentLayout);
    setFocusPolicy(Qt::FocusPolicy::ClickFocus);

    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameStyle(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setContentsMargins(0, 0, 0, 0);

    QWidget *widget = new QWidget;
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    widget->setContentsMargins(0, 0, 0, 0);

    mainContentLayout->addWidget(m_scrollArea);
    auto contentLayout = new QVBoxLayout();
    contentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    contentLayout->setSpacing(0);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    widget->setLayout(contentLayout);
    m_scrollArea->setWidget(widget);

    initUserInfo(contentLayout);
    initSetting(contentLayout);
}

AccountsDetailWidget::~AccountsDetailWidget()
{

}

void AccountsDetailWidget::setAccountModel(UserModel *model)
{
    if (!model) return;

    m_userModel = model;
    //TODO: 关联配置改变信号，控制自动登陆开关/无密码登陆开关显隐
    //TODO: 控制
}

void AccountsDetailWidget::initUserInfo(QVBoxLayout *layout)
{
    layout->addSpacing(35);
    AvatarWidget *avatar = new AvatarWidget;
    layout->addWidget(avatar, 0, Qt::AlignTop | Qt::AlignHCenter);

    avatar->setAvatarPath(m_curUser->currentAvatar());
    avatar->setFixedSize(80, 80);
    avatar->setArrowed(false);

    QLabel *shortName = new QLabel;
    shortName->setEnabled(false);
    shortName->setText(m_curUser->name());
    QLabel *shortnameBtn = new QLabel(this);
    shortnameBtn->setPixmap(QIcon::fromTheme("dcc_avatar").pixmap(12, 12));

    QHBoxLayout *shortnameLayout = new QHBoxLayout;
    shortnameLayout->setMargin(0);
    shortnameLayout->setAlignment(Qt::AlignHCenter);
    shortnameLayout->addWidget(shortnameBtn);
    shortnameLayout->addSpacing(3);
    shortnameLayout->addWidget(shortName);
    layout->addSpacing(5);
    layout->addLayout(shortnameLayout);

    m_fullName = new QLabel;
    m_fullName->setContentsMargins(0, 6, 0, 6);

    auto fullname = m_curUser->fullname();
    m_fullName->setEnabled(true);
    m_fullName->setText(fullname.toHtmlEscaped());

    QHBoxLayout *fullnameLayout = new QHBoxLayout;
    fullnameLayout->setSpacing(5);
    fullnameLayout->setAlignment(Qt::AlignHCenter);
    fullnameLayout->addWidget(m_fullName);
    layout->addLayout(fullnameLayout);

    connect(m_curUser, &User::currentAvatarChanged, avatar, &AvatarWidget::setAvatarPath);
    connect(m_curUser, &User::nameChanged, shortName, &QLabel::setText);
}

void AccountsDetailWidget::initSetting(QVBoxLayout *layout)
{
    QHBoxLayout *modifydelLayout = new QHBoxLayout;
    modifydelLayout->setContentsMargins(10, 0, 10, 0);
    modifydelLayout->addWidget(m_modifyPassword);
    modifydelLayout->addSpacing(10);
    modifydelLayout->addWidget(m_deleteAccount);
    layout->addSpacing(40);
    layout->addLayout(modifydelLayout);

    m_autoLogin = new SwitchWidget;
    m_autoLogin->setTitle("自动登录");
    m_nopasswdLogin = new SwitchWidget;
    m_nopasswdLogin->setTitle("修改密码");
    SettingsGroup *loginGrp = new SettingsGroup(nullptr, SettingsGroup::GroupBackground);

    loginGrp->getLayout()->setContentsMargins(0, 0, 0, 0);
    loginGrp->setContentsMargins(10, 10, 10, 10);
    loginGrp->layout()->setMargin(0);
    loginGrp->appendItem(m_autoLogin);
    loginGrp->appendItem(m_nopasswdLogin);
    layout->addSpacing(20);

    layout->addWidget(loginGrp);

    //修改密码
    connect(m_modifyPassword, &QPushButton::clicked, [ = ] {
        Q_EMIT requestShowPwdSettings(m_curUser);
    });


}
