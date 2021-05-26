#include "widgets/contentwidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QResizeEvent>
#include <QTimer>
#include <QDebug>
#include <QScroller>
#include <QScrollBar>
#include <QScrollArea>
#include <QApplication>
#include <QWheelEvent>
#include <QPropertyAnimation>

#include "widgets/labels/tipslabel.h"

#include "widgets/separator.h"
#include "widgets/backbutton.h"

DWIDGET_USE_NAMESPACE

namespace {

const int DEFAULT_SPEED_TIME = 2;

}  // namespace

namespace dcc {

ContentWidget::ContentWidget(QWidget *parent)
    : QWidget(parent)
    , m_content(nullptr)
    , m_speedTime(DEFAULT_SPEED_TIME)
{
    m_backBtn = new dcc::widgets::BackButton;
    m_backBtn->setAccessibleName("Back");

    m_title = new QLabel;
    m_title->setObjectName("ContentTitle");
    m_title->setAlignment(Qt::AlignCenter);

    m_contentTopLayout = new QVBoxLayout;
    m_contentTopLayout->setMargin(0);
    m_contentTopLayout->setSpacing(0);

    m_contentArea = new QScrollArea;
    m_contentArea->setWidgetResizable(true);
    m_contentArea->setFrameStyle(QFrame::NoFrame);
    m_contentArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_contentArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_contentArea->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    m_contentArea->setContentsMargins(0, 0, 0, 0);

    QScroller::grabGesture(m_contentArea->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller *scroller = QScroller::scroller(m_contentArea);
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);

#ifndef WINDOW_MODE
    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->addWidget(m_backBtn);
    titleLayout->addWidget(m_title);
    titleLayout->setContentsMargins(0, 0, m_backBtn->width(), 0);
#endif

    QVBoxLayout *centralLayout = new QVBoxLayout;
#ifndef WINDOW_MODE
    centralLayout->addLayout(titleLayout);
    centralLayout->addSpacing(15);
#endif
    centralLayout->addLayout(m_contentTopLayout);
    centralLayout->addWidget(m_contentArea);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);

    connect(m_backBtn, &dcc::widgets::BackButton::clicked, this, &ContentWidget::back);

    setLayout(centralLayout);
    setObjectName("ContentWidget");

    setBackgroundRole(QPalette::Base);
}

ContentWidget::~ContentWidget()
{
    QScroller *scroller = QScroller::scroller(m_contentArea->viewport());
    if (scroller) {
        scroller->stop();
        scroller->deleteLater();
    }
}

void ContentWidget::setTitle(const QString &title)
{
    m_title->setText(title);

    setAccessibleName(title);
}

void ContentWidget::sendBackSignal()
{
    Q_EMIT back();
}

QWidget *ContentWidget::setContent(QWidget *const w)
{
    QWidget *lastWidget = m_content;

    if (lastWidget)
        lastWidget->removeEventFilter(this);

    m_content = w;
    m_content->installEventFilter(this);
    m_contentArea->setWidget(m_content);

    return lastWidget;
}

void ContentWidget::scrollTo(int dy)
{
    m_contentArea->verticalScrollBar()->setValue(m_contentArea->verticalScrollBar()->value() + dy);
}

void ContentWidget::resizeEvent(QResizeEvent *event)
{
    if (m_content)
        m_content->setFixedWidth(m_contentArea->width());
    QWidget::resizeEvent(event);
}

}
