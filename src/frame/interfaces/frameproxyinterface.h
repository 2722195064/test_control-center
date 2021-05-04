#pragma once

#include "namespace.h"

class QWidget;
class QString;

namespace DCC_NAMESPACE {
class ModuleInterface;
class FrameProxyInterface
{
// TODO??
public:
    enum PushType {
        Replace,
        CoverTop,
        Normal,
        DirectTop,
        Count
    };

public:
    // 弹出下一页
    virtual void pushWidget(ModuleInterface *const inter, QWidget *const w) = 0;
    virtual void popWidget(ModuleInterface *const inter) = 0;

public:
    ModuleInterface *currModule() const { return m_currModule; }

protected:
    void setCurrModule(ModuleInterface *const m) { m_currModule = m; }

private:
    ModuleInterface *m_currModule{nullptr};
};
}
