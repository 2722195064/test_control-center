#include "window/mainwindow.h"

#include <DDBusSender>
#include <dapplication.h>
#include <DApplicationSettings>
#include <DApplication>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

int main(int argc, char *argv[])
{
    DApplication *app = DApplication::globalApplication(argc, argv);


    return app->exec();
}

