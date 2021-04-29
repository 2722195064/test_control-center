/*
主窗口抽象类
*/

#include <QWidget>
class ModuleInterface;
class FrameProxyInterface
{
public:
    // 弹出下一页
    virtual void pushWidget(ModuleInterface *const inter, QWidget *const w) = 0;
    virtual void popWidget(ModuleInterface *const inter) = 0;

};
