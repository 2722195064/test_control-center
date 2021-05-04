/*
 ModuleInterface作为每个规范每个Module的接口，每个Module实现必须实现其所有虚函数。
*/

#pragma once
#include "namespace.h"
#include "frameproxyinterface.h"

#include <QIcon>
#include <QtWidgets>
#include <QtCore>

namespace DCC_NAMESPACE {

class ModuleInterface
{
public:
    ModuleInterface() = default;
    ModuleInterface(FrameProxyInterface *frameProxy) : m_frameProxy(frameProxy) {}
    virtual ~ModuleInterface() {}

    void setFrameProxy(FrameProxyInterface *frameProxy) { m_frameProxy = frameProxy; }

    // 用于初始化模块预加载
    virtual void preInitialize(bool sync = false,FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) {Q_UNUSED(sync)}

    // 模块初始化
    virtual void initialize() = 0;

    // module name
    virtual const QString name() const = 0;

    // 当模块第一次被点击进入时，active会被调用,
    // 如果是插件,重载的时候必须声明为slots,否则加载不了
    virtual void active() {}

    // 设置module图标
    virtual QIcon icon() const {
        return QIcon::fromTheme(QString("dcc_nav_%1").arg(name()));
    }

protected:
    FrameProxyInterface *m_frameProxy { nullptr };

};
}

#define ModuleInterface_iid "com.deepin.dde.ControlCenter.module/1.0"
Q_DECLARE_INTERFACE(DCC_NAMESPACE::ModuleInterface, ModuleInterface_iid)
//#define ModuleInterface_iid
