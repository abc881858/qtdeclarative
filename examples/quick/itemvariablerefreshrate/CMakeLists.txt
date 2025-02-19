# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(itemvariablerefreshrate LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/quick/itemvariablerefreshrate")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

qt_add_executable(itemvariablerefreshrateexample
    main.cpp
)

set_target_properties(itemvariablerefreshrateexample PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)


target_link_libraries(itemvariablerefreshrateexample PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
)

qt_add_qml_module(itemvariablerefreshrateexample
    URI itemvariablerefreshrate
    VERSION 1.0
    AUTO_RESOURCE_PREFIX
    QML_FILES
        "itemvariablerefreshrate.qml"
    RESOURCES
        "content/qt_logo.png"
)

install(TARGETS itemvariablerefreshrateexample
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
