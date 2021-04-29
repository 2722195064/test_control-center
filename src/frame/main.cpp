#include "window/mainwindow.h"

#include <DDBusSender>
#include <dapplication.h>
#include <DApplicationSettings>
#include <DApplication>

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <unistd.h>

#include <QWidget>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

static MainWindow *gwm{nullptr};

int main(int argc, char *argv[])
{
    DApplication *app = DApplication::globalApplication(argc, argv);
    if (!app->setSingleInstance(QString("dde-control-center_%1").arg(getuid()))) {
        qDebug() << "set single instance failed!";
        return -1;
    }
    app->setOrganizationName("deepin");
    app->setApplicationName("test-control-center");

    // TODO: 添加命令行
    QCommandLineOption showOption(QStringList() << "s" << "show", "show control center(hide for default).");

    QCommandLineParser parser;
    parser.setApplicationDescription("DDE Control Center");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(showOption);

    // 显示主窗口
    //TODO:GSetting
    QRect mwRect(0, 0, 820, 634);

    // TODO??DCC_NAMESPACE::
    MainWindow mw;
    QDBusConnection con = QDBusConnection::sessionBus();
    mw.show();
    mw.initAllmodule();

    // #ifdef QT_DEBUG
//    DDBusSender()
//            .service("com.deepin.dde.ControlCenter")
//            .interface("com.deepin.dde.ControlCenter")
//            .path("/com/deepin/dde/ControlCenter")
//            .method("Show")
//            .call();

    return app->exec();
}

