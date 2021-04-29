#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "frameproxyinterface.h"

#include <DBackgroundGroup>
#include <DMainWindow>
#include <QDBusContext>
#include <DListView>
#include <DSearchEdit>

DWIDGET_USE_NAMESPACE

// 添加声明
QT_BEGIN_NAMESPACE
class QHBoxLayout;
class QStandardItemModel;
//class DListView;
QT_END_NAMESPACE

class MainWindow : public DMainWindow, public FrameProxyInterface, protected QDBusContext
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() override;

    void pushWidget(ModuleInterface *const inter, QWidget *const w) override;
    void popWidget(ModuleInterface *const inter) override;

public:
    void initAllmodule(const QString &m = "");
    void onFirstItemClick(const QModelIndex &index);


private:
    QHBoxLayout *m_contentLayout; //内容布局
    QHBoxLayout *m_rightContentLayout;
    DListView *m_navView;  // 顶部页面
    DBackgroundGroup *m_rightView;  // 右侧视图
    QStandardItemModel *m_navModel; // 顶部视图模型
    DSearchEdit *m_searchWidget; // 搜索框
    DIconButton *m_backwardBtn;


};

#endif // MAINWINDOW_H
