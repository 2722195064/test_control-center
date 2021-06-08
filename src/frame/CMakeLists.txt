cmake_minimum_required(VERSION 3.7)

set(VERSION 4.0)

set(BIN_NAME test-control-center)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS "-g -Wall -Wl,--as-need -fPIE")

set(DEFINED_LIST)

#本地文件
set(FRAME_FILES
    main.cpp
    dbuscontrolcenterservice.cpp)

set(INTERFACES_FILES
    interfaces/frameproxyinterface.h
    interfaces/moduleinterface.h
#    include/namespace.h
)

# 加载 widgets
set(WIDGETS_FILES
                widgets/backbutton.cpp
                widgets/basiclistdelegate.cpp
                ../..
                ../../include/widgets/basiclistdelegate.h
                widgets/buttontuple.cpp
                widgets/dccslider.cpp
                ../../include/widgets/dccslider.h
                widgets/dccsliderannotated.cpp
                ../../include/widgets/dccsliderannotated.h
                ../../include/widgets/labels/largelabel.h
                ../../include/widgets/labels/normallabel.h
                ../../include/widgets/labels/smalllabel.h
                ../../include/widgets/labels/tipslabel.h
                widgets/lineeditwidget.cpp
                widgets/loadingindicator.cpp
                ../../include/widgets/loadingindicator.h
                widgets/loadingnextpagewidget.cpp
                widgets/nextbutton.cpp
                widgets/optionlistpage.cpp
                widgets/searchinput.cpp
                ../../include/widgets/searchinput.h
                widgets/separator.cpp
                ../../include/widgets/separator.h
                widgets/settingsheaderitem.cpp
                ../../include/widgets/settingsheaderitem.h
                widgets/settingsitem.cpp
                ../../include/widgets/settingsitem.h
                widgets/spinboxwidget.cpp
                ../../include/widgets/spinboxwidget.h
                widgets/buttoneditwidget.cpp
                ../../include/widgets/buttoneditwidget.h
                widgets/tipsitem.cpp
                ../../include/widgets/tipsitem.h
                widgets/titlelabel.cpp
                ../../include/widgets/titlelabel.h
                widgets/titlevalueitem.cpp
                ../../include/widgets/titlevalueitem.h
                widgets/translucentframe.cpp
                ../../include/widgets/translucentframe.h
                widgets/settingsgroup.cpp
                widgets/switchwidget.cpp
                widgets/comboxwidget.cpp
                ../../include/widgets/comboxwidget.h
                widgets/themeitempic.cpp
                ../../include/widgets/themeitempic.h
                ../../include/widgets/utils.h
                ../../include/widgets/backbutton.h
                widgets/basiclistmodel.cpp
                ../../include/widgets/basiclistmodel.h
                widgets/basiclistview.cpp
                ../../include/widgets/basiclistview.h
                ../../include/widgets/buttontuple.h
                widgets/comboboxwidget.cpp
                ../../include/widgets/comboboxwidget.h
                widgets/contentwidget.cpp
                ../../include/widgets/contentwidget.h
                widgets/editablenextpagewidget.cpp
                ../../include/widgets/editablenextpagewidget.h
                widgets/filechoosewidget.cpp
                ../../include/widgets/filechoosewidget.h
                ../../include/widgets/lineeditwidget.h
                ../../include/widgets/loadingnextpagewidget.h
                ../../include/widgets/nextbutton.h
                widgets/nextpagewidget.cpp
                ../../include/widgets/nextpagewidget.h
                widgets/optionitem.cpp
                ../../include/widgets/optionitem.h
                ../../include/widgets/optionlistpage.h
                ../../include/widgets/settingsgroup.h
                widgets/settingshead.cpp
                ../../include/widgets/settingshead.h
                ../../include/widgets/switchwidget.h
                widgets/themeitem.cpp
                ../../include/widgets/themeitem.h
                widgets/titledslideritem.cpp
                ../../include/widgets/titledslideritem.h
                widgets/timeoutdialog.cpp
                ../../include/widgets/timeoutdialog.h
                widgets/multiselectlistview.cpp
                 ../../include/widgets/multiselectlistview.h
                widgets/powerdisplaywidget.cpp
                ../../include/widgets/powerdisplaywidget.h
                widgets/widgets.qrc
)

set(WINDOW_FILES
    window/mainwindow.h
    window/mainwindow.cpp
    window/insertplugin.h
    window/insertplugin.cpp
    window/utils.h
)

# sync
set(SYNC_FILES
    window/modules/sync/syncmodule.cpp
    window/modules/sync/syncwidget.cpp
    )

# accounts 账户
set(ACCOUNTS_FILES
    modules/accounts/user.cpp
    modules/accounts/usermodel.cpp
    modules/accounts/avatarwidget.cpp
    modules/accounts/avatardel.cpp
    modules/accounts/creationresult.cpp
    window/modules/accounts/accountsmodule.cpp
    window/modules/accounts/accountswidget.cpp
    window/modules/accounts/accountsworker.cpp
    window/modules/accounts/accountsdetailwidget.cpp
    window/modules/accounts/createaccountpage.cpp
    )

# bluetooth 蓝牙
set(BLUETOOTH_FILES
    modules/bluetooth/bluetoothmodel.cpp
    modules/bluetooth/bluetoothworker.cpp
    modules/bluetooth/device.cpp
    modules/bluetooth/adapter.cpp
    window/modules/bluetooth/detailpage.cpp
    window/modules/bluetooth/titleedit.cpp
    window/modules/bluetooth/bluetoothwidget.cpp
    window/modules/bluetooth/bluetoothmodule.cpp
    window/modules/bluetooth/adapterwidget.cpp
    window/modules/bluetooth/devicesettingsitem.cpp
)

set(SRCS
    ${FRAME_FILES}
    ${INTERFACES_FILES}
    ${WINDOW_FILES}
    ${ACCOUNTS_FILES}
    ${BLUETOOTH_FILES}
    ${SYNC_FILES}
)

# Find library
find_package(PkgConfig REQUIRED)
find_package(DtkWidget REQUIRED)
find_package(DtkGui REQUIRED)
find_package(PolkitQt5-1)  # 授权认证
find_package(DtkCMake REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets Multimedia DBus Network Svg REQUIRED)

set(DEEPIN_PW_CHECK libdeepin_pw_check.so)

#  pkg-config 是否存在指定的 .pc 文件
pkg_check_modules(QGSettings REQUIRED gsettings-qt)
pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)


set(INCLUDES)

# 添加连接库
add_library(dccwidgets SHARED
    ${WIDGETS_FILES}
)

# 导入目标头文件
target_include_directories(dccwidgets PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${DtkWidget_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
)


# 目标连接库
target_link_libraries(dccwidgets PRIVATE
    ${DtkWidget_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
)

# 添加可执行文件
add_executable(${BIN_NAME} ${SRCS})
# 导入目标目录
target_include_directories(${BIN_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${DtkWidget_INCLUDE_DIRS}
    ${XCB_EWMH_INCLUDE_DIRS}
    ${DFrameworkDBus_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${PROJECT_BINARY_DIR}
    ${QGSettings_INCLUDE_DIRS}
    ${DDE-Network-Utils_INCLUDE_DIRS}
    ${Gio-2.0_INCLUDE_DIRS}
    ${LibNM_INCLUDE_DIRS}
    ${Qt5X11Extras_INCLUDE_DIRS}
    ${KF5_QT_INCLUDE_DIRS}
    ${INCLUDES}
)

# 将目标文件与库文件进行链接
target_link_libraries(${BIN_NAME} PRIVATE
    dccwidgets
    ${XCB_EWMH_LIBRARIES}
    ${DFrameworkDBus_LIBRARIES}
    ${DtkWidget_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5Concurrent_LIBRARIES}
    ${Qt5X11Extras_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    ${Qt5Network_LIBRARIES}
    ${Qt5Multimedia_LIBRARIES}
    ${QGSettings_LIBRARIES}
    ${DDE-Network-Utils_LIBRARIES}
    ${Gio-2.0_LIBRARIES}
    ${LibNM_LIBRARIES}
    ${KF5_QT_LIBRARIES}
    ${Qt5Svg_LIBRARIES}
    ${DEEPIN_PW_CHECK}
    crypt
    ${LIBS}
    PolkitQt5-1::Agent
)

# bin
install(TARGETS ${BIN_NAME} DESTINATION bin)
install(TARGETS dccwidgets LIBRARY DESTINATION lib)

set(Qt_LIBS
${Qt_LIBS}
)
