#include "titleedit.h"

#include <DToolButton>

#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <DLineEdit>
#include <DDesktopServices>

DWIDGET_USE_NAMESPACE

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::bluetooth;

TitleEdit::TitleEdit(QWidget *parent)
    : QWidget(parent)
    , m_name(new QLabel)
    , m_lineEdit(new DLineEdit)
{
    QHBoxLayout *mainlayout = new QHBoxLayout;
    m_lineEdit->lineEdit()->setVisible(false);
    mainlayout->addWidget(m_name);
    mainlayout->addWidget(m_lineEdit);
    mainlayout->addSpacing(5);
    DToolButton *editWidget = new DToolButton(this);
    editWidget->setIcon(QIcon::fromTheme("dcc_edit"));
    mainlayout->addWidget(editWidget);
    mainlayout->addStretch();
    mainlayout->setMargin(0);
    mainlayout->setSpacing(0);
    setLayout(mainlayout);

    connect(m_lineEdit, &DLineEdit::editingFinished, this, [this, editWidget](){
        this->setName();
        editWidget->setVisible(true);
    });
    connect(m_lineEdit, &DLineEdit::textChanged, this, [=](const QString &str) {
        if (str.length() > 32) {
            m_lineEdit->lineEdit()->backspace();
            DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
        }
    });
    connect(editWidget, &DToolButton::clicked, this, [this, editWidget](){
        editWidget->setVisible(false);
        this->setEdit();
    });
}

void TitleEdit::setName()
{
    m_lineEdit->lineEdit()->setVisible(false);
    m_name->setVisible(true);
    if (m_name->text() != m_lineEdit->text()) {
        if (!m_lineEdit->text().isEmpty()) {
            m_name->setText(m_lineEdit->text());
        }
        if (!m_lineEdit->text().isEmpty()) {
            Q_EMIT requestSetBluetoothName(m_lineEdit->text());
        } else {
            m_lineEdit->setText(m_name->text());
        }
    }
    m_name->setFocus();
}

void TitleEdit::setEdit()
{
    m_name->setVisible(false);
    m_lineEdit->lineEdit()->setVisible(true);
    m_lineEdit->lineEdit()->setFocus();
}

void TitleEdit::setTitle(const QString &title)
{
    m_name->setText(title);
    m_lineEdit->setText(title);
}
