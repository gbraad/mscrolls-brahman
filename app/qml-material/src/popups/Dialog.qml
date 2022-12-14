/*
 * QML Material - An application framework implementing Material Design.
 *
 * Copyright (C) 2014-2016 Michael Spencer <sonrisesoftware@gmail.com>
 *               2015 Bogdan Cuza <bogdan.cuza@hotmail.com>
 *               2015 Mikhail Ivchenko <ematirov@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

import QtQuick 2.4
import QtQuick.Layouts 1.1
import Material 0.3
import Material.Extras 0.1

/*!
   \qmltype Dialog
   \inqmlmodule Material
   \brief Dialogs inform users about critical information, require users to make
   decisions, or encapsulate multiple tasks within a discrete process
 */
PopupBase {
    id: dialog

    overlayLayer: "dialogOverlayLayer"
    overlayColor: Qt.rgba(0, 0, 0, 0.3)

    opacity: showing ? 1 : 0
    visible: opacity > 0

    width: Math.max(minimumWidth,
                    content.contentWidth + 2 * contentMargins)

    height: Math.min(parent.height - 64 * Units.dp,
                     headerView.height +
                     content.contentHeight +
                     (floatingActions ? 0 : buttonContainer.height))

    property int contentMargins: 24 * Units.dp
    property int vmargin: Math.max(headerView.height + (floatingActions ? 0 : buttonContainer.height) + contentMargins, 64*Units.dp)

    property int minimumWidth: Device.isMobile ? 280 * Units.dp : 300 * Units.dp

    property alias title: titleLabel.text
    property alias text: textLabel.text
    property color backgroundColor: Theme.backgroundColor
    property bool centerTitle: false

    /*!
       \qmlproperty Button negativeButton
       The negative button, displayed as the leftmost button on the right of the dialog buttons.
       This is usually used to dismiss the dialog.
     */
    property alias negativeButton: negativeButton

    /*!
       \qmlproperty Button primaryButton
       The primary button, displayed as the rightmost button in the dialog buttons row. This is
       usually used to accept the dialog's action.
     */
    property alias positiveButton: positiveButton

    property string negativeButtonText: "Cancel"
    property string positiveButtonText: "Ok"
    property alias positiveButtonEnabled: positiveButton.enabled

    property bool hasActions: true
    property bool floatingActions: false
    property bool allowFlick: true

    default property alias dialogContent: column.data

    signal accepted()
    signal rejected()

    anchors {
        centerIn: parent
        verticalCenterOffset: showing ? 0 : -(dialog.height/3)

        Behavior on verticalCenterOffset {
            NumberAnimation { duration: 200 }
        }
    }

    Behavior on opacity {
        NumberAnimation { duration: 200 }
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_Escape) {
            closeKeyPressed(event)
        }
    }

    Keys.onReleased: {
        if (event.key === Qt.Key_Back) {
            closeKeyPressed(event)
        }
    }

    function closeKeyPressed(event) {
        if (dialog.showing) {
            if (dialog.dismissOnTap) {
                dialog.close()
            }
            event.accepted = true
        }
    }

    function show() {
        open()
    }

    View {
        id: dialogContainer

        anchors.fill: parent
        elevation: 5
        radius: 2 * Units.dp
        backgroundColor: dialog.backgroundColor

        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: false

            onClicked: {
                mouse.accepted = false
            }
        }

        /*
        Rectangle {
            anchors.fill: content
        }
        */

        Flickable {
            id: content

            contentWidth: column.implicitWidth
            contentHeight: column.height + (column.height > 0 ? contentMargins : 0)
            clip: true

            anchors {
                left: parent.left
                right: parent.right
                top: headerView.bottom
                bottom: floatingActions ? parent.bottom : buttonContainer.top
            }

            interactive: contentHeight > height && allowFlick

            onContentXChanged: {
                if(contentX != 0 && contentWidth <= width)
                    contentX = 0
            }

            onContentYChanged: {
                if(contentY != 0 && contentHeight <= height)
                    contentY = 0
            }

            Column {
                id: column
                anchors {
                    left: parent.left
                    leftMargin: contentMargins
                }

                width: content.width - 2 * contentMargins
                spacing: 8 * Units.dp
            }
        }

        Scrollbar {
            flickableItem: content
        }

        Item {
            // area for the header text
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }

            height: headerView.height

            View {
                //backgroundColor: Theme.backgroundColor
                elevation: content.atYBeginning ? 0 : 1
                fullWidth: true
                radius: dialogContainer.radius

                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                }

                height: parent.height
            }
        }


        Column {
            id: headerView

            spacing: 0

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top

                leftMargin: contentMargins
                rightMargin: contentMargins
            }

            Item {
                width: parent.width
                height: contentMargins
                visible: titleLabel.visible || textLabel.visible
            }

            Label {
                id: titleLabel

                width: parent.width
                wrapMode: Text.Wrap
                style: "title"
                visible: title != ""
                horizontalAlignment: centerTitle ? Text.AlignHCenter : Text.AlignLeft
            }

            Item {
                width: parent.width
                height: 20 * Units.dp
                visible: titleLabel.visible
            }

            Label {
                id: textLabel

                width: parent.width
                wrapMode: Text.Wrap
                style: "dialog"
                color: Theme.subTextColor
                visible: text != ""
            }

            Item {
                width: parent.width
                height: contentMargins
                visible: textLabel.visible
            }
        }

        Item {
            id: buttonContainer

            anchors {
                bottom: parent.bottom
                right: parent.right
                left: parent.left
            }

            height: hasActions ? 52 * Units.dp : 2 * Units.dp

            View {
                id: buttonView

                height: parent.height
                backgroundColor: floatingActions ? "transparent" : dialog.backgroundColor
                elevation: content.atYEnd ? 0 : 1
                fullWidth: true
                radius: dialogContainer.radius
                elevationInverted: true

                anchors {
                    bottom: parent.bottom
                    right: parent.right
                    left: parent.left
                }

                MButton {
                    id: negativeButton

		    // VW_CHANGE
                    visible: hasActions && negativeButtonText.length > 0
                    text: negativeButtonText
                    textColor: Theme.accentColor
                    context: "dialog"

                    anchors {
                        verticalCenter: parent.verticalCenter
                        right: positiveButton.visible ? positiveButton.left : parent.right
                        rightMargin: 8 * Units.dp
                    }

                    onClicked: {
                        close();
                        rejected();
                    }
                }

                MButton {
                    id: positiveButton

                    visible: hasActions
                    text: positiveButtonText
                    textColor: Theme.accentColor
                    context: "dialog"

                    anchors {
                        verticalCenter: parent.verticalCenter
                        rightMargin: 8 * Units.dp
                        right: parent.right
                    }

                    onClicked: {
                        close()
                        accepted();
                    }
                }
            }
        }
    }
}
