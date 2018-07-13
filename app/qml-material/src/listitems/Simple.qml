/*
 * QML Material - An application framework implementing Material Design.
 *
 * Copyright (C) 2014-2016 Michael Spencer <sonrisesoftware@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

import QtQuick 2.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.1
import Material 0.3

/*!
   \qmltype Standard
   \inqmlmodule Material.ListItems

 */
BaseListItem
{
    id: listItem

    implicitHeight: 48 * Units.dp
    height: 48 * Units.dp
    width: parent.width

    property alias text: label.text
    property alias textColor: label.color
    property alias label: label
    property bool supportLinks: false

    dividerInset: 0

    Text
    {
        id: label

        anchors
        {
            fill: parent
            leftMargin: listItem.margins
            rightMargin: listItem.margins
        }

        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        color: listItem.selected ? Theme.primaryColor : Theme.textColor
        font.pixelSize: 16*Units.dp
        font.family: Theme.fontFamily
        onLinkActivated: if (supportLinks) Qt.openUrlExternally(link)
    }
}
