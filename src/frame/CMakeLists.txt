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
)

set(WINDOW_FILES

    window/mainwindow.h
    window/mainwindow.cpp
    window/insertplugin.h
    window/insertplugin.cpp
)

# sync
set(SYNC_FILES
    window/modules/sync/syncmodule.cpp
    window/modules/sync/syncwidget.cpp

    )

# accounts 账户
set(ACCOUNTS_FILES
    window/modules/accounts/accountsmodule.cpp
    window/modules/accounts/accountswidget.cpp
    window/modules/accounts/accountsworker.cpp
    )

set(SRCS
    ${FRAME_FILES}
    ${INTERFACES_FILES}
    ${WINDOW_FILES}
    ${ACCOUNTS_FILES}
    ${SYNC_FILES}
)

# Find library
find_package(PkgConfig REQUIRED)
find_package(DtkWidget REQUIRED)
find_package(DtkCMake REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets Multimedia DBus Network Svg REQUIRED)

# 添加连接库
#add_library(dccwidgets SHARED
#    ${WIDGETS_FILES}
#)

# 导入目标头文件
#target_include_directories(dccwidgets PUBLIC
#    ${CMAKE_SOURCE_DIR}/include
#    ${DtkWidget_INCLUDE_DIRS}
#    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
#)


# 目标连接库
#target_link_libraries(dccwidgets PRIVATE
#    ${DtkWidget_LIBRARIES}
#    ${Qt5Widgets_LIBRARIES}
#)

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
    #dccwidgets
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
)

set(Qt_LIBS
${Qt_LIBS}
)
