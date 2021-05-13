#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "interfaces/frameproxyinterface.h"

#include <DBackgroundGroup>
#include <DMainWindow>
#include <QDBusContext>
#include <DListView>
#include <DSearchEdit>
#include <QStack>

DWIDGET_USE_NAMESPACE

//namespace dcc {
//namespace widgets {
//class MultiSelectListView;
//}
//}

// 添加声明
QT_BEGIN_NAMESPACE
class QHBoxLayout;
class QStandardItemModel;
class QWidget;
//class DListView;
QT_END_NAMESPACE


namespace DCC_NAMESPACE {
class ModuleInterface;
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
    void popAllWidgets(int place = 0);
    void popWidget();

private:
    void resetNavList(bool isIconMode);
    void modulePreInitialize(const QString &m = nullptr);
    void pushNormalWidget(ModuleInterface *const inter, QWidget *const w);  //exchange third widget : push new widget


private:
    QHBoxLayout *m_contentLayout; //内容布局
    QHBoxLayout *m_rightContentLayout; // 二三级界面
    DListView *m_navView;  // 顶部页面
    DBackgroundGroup *m_rightView;  // 右侧视图
    QStandardItemModel *m_navModel; // 顶部视图模型
    DSearchEdit *m_searchWidget; // 搜索框
    DIconButton *m_backwardBtn; // 返回


    bool m_existenceInit{false};
    QList<ModuleInterface *> m_initList;
    QList<QPair<ModuleInterface *, QString>> m_modules; // 模块键值对列表
    QStack<QPair<ModuleInterface *, QWidget *>> m_contentStack;

};
}


#endif // MAINWINDOW_H
