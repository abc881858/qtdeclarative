// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

import QtQuick

Rectangle {
    id: root
    width: 600
    height: 540
    objectName: "root"
    color: "#222222"

    Grid {
        objectName: "grid"
        anchors.fill: parent
        spacing: 10
        columns: 6
        Repeater {
            id: top
            objectName: "top"
            model: 6

            delegate: Slider {
                objectName: label
                label: "Drag Knob " + index
                width: 140
            }
        }
        Repeater {
            id: bottom
            objectName: "bottom"
            model: 6

            delegate: DragAnywhereSlider {
                objectName: label
                label: "Drag Anywhere " + index
                width: 140
            }
        }
    }
}
