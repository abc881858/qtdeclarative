# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

qt_add_library(qqmljsscope_test_module STATIC)
qt_autogen_tools_initial_setup(qqmljsscope_test_module)

# use PUBLIC everywhere to simplify the build of the test binary
target_include_directories(qqmljsscope_test_module PUBLIC cpptypes/)
target_link_libraries(qqmljsscope_test_module PUBLIC Qt::Core Qt::Qml Qt::Gui)

qt6_add_qml_module(qqmljsscope_test_module
    VERSION 1.0
    URI QQmlJSScopeTests
    AUTO_RESOURCE_PREFIX
    SOURCES
        singleton.h singleton.cpp
        extensiontypes.h
        typewithproperties.h
)

qt_autogen_tools_initial_setup(qqmljsscope_test_moduleplugin)
