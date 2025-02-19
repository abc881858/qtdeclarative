# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(valuesource LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/qml/referenceexamples/valuesource")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml)

qt_add_executable(valuesource
    birthdayparty.cpp birthdayparty.h
    happybirthdaysong.cpp happybirthdaysong.h
    main.cpp
    person.cpp person.h
)

set_target_properties(valuesource PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(valuesource PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Qml
)

qt_add_qml_module(valuesource
    URI People
    VERSION 1.0
    QML_FILES example.qml
    NO_RESOURCE_TARGET_PATH
)

install(TARGETS valuesource
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
