/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Quick Controls module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.4
import QtQuick.Controls 2.0

AbstractSlider {
    id: control

    implicitWidth: Math.max(background ? background.implicitWidth : 0,
                            Math.max(track ? track.implicitWidth : 0,
                                     handle ? handle.implicitWidth : 0) + leftPadding + rightPadding)
    implicitHeight: Math.max(background ? background.implicitHeight : 0,
                             Math.max(track ? track.implicitHeight : 0,
                                      handle ? handle.implicitHeight : 0) + topPadding + bottomPadding)

    Accessible.pressed: pressed
    Accessible.role: Accessible.Slider

    padding: style.padding

    handle: Rectangle {
        implicitWidth: 20
        implicitHeight: 20
        radius: width / 2
        border.width: control.activeFocus ? 2 : 1
        border.color: control.activeFocus ? style.focusColor : style.frameColor
        color: style.backgroundColor

        readonly property bool horizontal: control.orientation === Qt.Horizontal
        x: horizontal ? control.visualPosition * (control.width - width) : (control.width - width) / 2
        y: horizontal ? (control.height - height) / 2 : control.visualPosition * (control.height - height)

        Rectangle {
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            width: 12
            height: 12
            radius: width / 2

            color: Qt.tint(Qt.tint(style.accentColor,
                                   control.activeFocus ? style.focusColor : "transparent"),
                                   control.pressed ? style.pressColor : "transparent")
        }
    }

    track: Rectangle {
        readonly property bool horizontal: control.orientation === Qt.Horizontal
        implicitWidth: horizontal ? 120 : 6
        implicitHeight: horizontal ? 6 : 120
        x: horizontal ? control.leftPadding : (control.width - width) / 2
        y: horizontal ? (control.height - height) / 2 : control.topPadding
        width: horizontal ? parent.width - control.leftPadding - control.rightPadding : implicitWidth
        height: horizontal ? implicitHeight : parent.height - control.topPadding - control.bottomPadding

        radius: style.roundness
        border.color: style.frameColor
        color: style.backgroundColor
        scale: control.effectiveLayoutDirection === Qt.RightToLeft ? -1 : 1

        Rectangle {
            x: 2
            y: 2
            width: control.position * parent.width - 4
            height: 2

            color: style.accentColor
            radius: style.roundness
        }
    }
}
