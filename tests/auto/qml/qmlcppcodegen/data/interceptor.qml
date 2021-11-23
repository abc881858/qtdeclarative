/******************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt JavaScript to C++ compiler.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

import QtQuick
import TestTypes

Item {
    Behavior on width { PropertyAnimation {} }

    CppBaseClass {
        id: withBehavior
        Behavior on cppProp { PropertyAnimation {} }
        Component.onCompleted: cppProp = 200
    }
    height: withBehavior.cppProp

    CppBaseClass {
        id: withoutBehavior
        Component.onCompleted: cppProp = 100
    }
    x: withoutBehavior.cppProp

    Component.onCompleted: {
        width = 200
        y = 100
    }

    CppBaseClass {
        id: qPropertyBinder
        cppProp: withoutBehavior.cppProp + withBehavior.cppProp
    }

    property int qProperty1: qPropertyBinder.cppProp
    property int qProperty2: withoutBehavior.cppProp + withBehavior.cppProp
}
