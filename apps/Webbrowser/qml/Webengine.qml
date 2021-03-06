// Copyright (c) 2016-2018, EPFL/Blue Brain Project
//                          Raphael Dumusc <raphael.dumusc@epfl.ch>
import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0

import "qrc:/qml/core/."
import "qrc:/qml/core/style.js" as Style
import "qrc:/web/qml/."

Item {
    width: 640
    height: 480

    onWindowChanged: {
        // size constraints for the stream window
        window.minimumWidth = 640
        window.minimumHeight = 480
        window.maximumWidth = 3840
        window.maximumHeight = 2160
    }

    signal addressBarTextEntered(string url)

    Item {
        id: controlBar
        anchors.left: parent.left
        anchors.right: parent.right
        height: Style.windowTitleHeight

        RectangularGlow {
            id: focusEffect
            anchors.fill: addressBar
            cornerRadius: glowRadius
            color: Style.windowFocusGlowColor
            glowRadius: Style.windowFocusGlowRadius
            spread: Style.windowFocusGlowSpread
            visible: addressBar.focus
        }

        TextField {
            id: addressBar
            anchors.fill: parent
            anchors.margins: Style.windowBorderWidth
            font.pixelSize: 0.5 * (parent.height - 2 * Style.windowBorderWidth)

            placeholderText: "enter url or search terms"
            text: webbrowser.url

            onAccepted: addressBarTextEntered(text)
            onFocusChanged: focus ? Qt.inputMethod.show(
                                        ) : Qt.inputMethod.hide()

            Rectangle {
                color: "white"
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.margins: 0.1 * parent.height
                width: height
                ControlButton {
                    id: goButton
                    anchors.fill: parent
                    anchors.margins: 0.1 * parent.height
                    image: "qrc:/webbrowser/images/right_arrow.svg"
                    opacity: addressBar.text.length
                             > 0 ? Style.buttonsEnabledOpacity : Style.buttonsDisabledOpacity
                    enabled: true
                    onClicked: addressBarTextEntered(addressBar.text)
                }
            }
        }
    }

    WebBrowser {
        id: webbrowser
        anchors.top: controlBar.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        Connections {
            target: deflectgestures
            onSwipeLeft: webbrowser.goBack()
            onSwipeRight: webbrowser.goForward()
        }
    }

    Rectangle {
        id: addressBarFocusContext
        anchors.fill: webbrowser
        color: "black"
        opacity: addressBar.focus ? 0.5 : 0.0
        visible: opacity > 0.0
        Behavior on opacity {
            PropertyAnimation {
                easing.type: Easing.InOutQuad
                duration: 150
            }
        }
    }

    Loader {
        id: virtualKeyboard
        source: "qrc:/virtualkeyboard/InputPanel.qml"
        width: Math.min(Style.keyboardMaxSizePx,
                        Style.keyboardRelSize * parent.width)
        height: 0.25 * width
        anchors.horizontalCenter: parent.horizontalCenter
        y: Qt.inputMethod.visible ? Math.min(
                                        0.5 * parent.height,
                                        parent.height - height) : parent.height
        opacity: Qt.inputMethod.visible ? 1.0 : 0.0
        Behavior on y {
            PropertyAnimation {
                easing.type: Easing.InOutQuad
                duration: 150
            }
        }
        Behavior on opacity {
            PropertyAnimation {
                easing.type: Easing.InOutQuad
                duration: 150
            }
        }
        onStatusChanged: {
            if (status == Loader.Error)
                source = "qrc:/qml/core/MissingVirtualKeyboard.qml"
        }
    }
}
