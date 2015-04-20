import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import org.kde.kquickcontrolsaddons 2.0

ColumnLayout {
    id: delegate
    width: parent.width

    RowLayout {
        QIconItem {
            width: height
            height: inputText.contentHeight
            anchors.verticalCenter: parent.verticalCenter
            icon: 'audio-card'
        }

        Label {
            id: inputText
            Layout.fillWidth: true
            text: PulseObject.description
        }

        MuteButton {
            muted: PulseObject.muted
            onMutedChanged: PulseObject.muted = muted
        }
    }

    ColumnLayout {
        width: parent.width
        enabled: !PulseObject.muted

        RowLayout {
            Label {
                text: 'Port'
            }

            ComboBox {
                Layout.fillWidth: true
                model: PulseObject.ports
                textRole: "description"
                currentIndex: PulseObject.activePortIndex
                onActivated: PulseObject.activePortIndex = index
                Connections {
                    target: PulseObject
                    // FIXME: comobox for some reason doesn't update after init
                    onActivePortIndexChanged: parent.currentIndex = PulseObject.activePortIndex
                }
            }
        }

        VolumeSlider {}
    }

    ListItemSeperator { view: delegate.ListView.view }
}