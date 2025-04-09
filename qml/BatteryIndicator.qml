// BatteryIndicator.qml
import QtQuick 6.2
import QtQuick.Layouts

Item {
    id: root
    width: 90
    height: 45

    property real voltage: 12.5

    RowLayout {
        anchors.fill: parent
        spacing: 20
        Layout.alignment: Qt.AlignHCenter

        Canvas {
            id: batteryCanvas
            width: 90
            height: 45

            onPaint: {
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)

                var level = Math.min(Math.max(root.voltage / 13.0, 0), 1)
                var fillWidth = level * (width - 10)

                ctx.strokeStyle = "white"
                ctx.lineWidth = 2
                ctx.strokeRect(1, 5, width - 10, height - 10)

                ctx.fillStyle = "white"
                ctx.fillRect(width - 8, height / 2 - 6, 6, 12)

                var r = Math.floor((1 - level) * 255)
                var g = Math.floor(level * 255)
                ctx.fillStyle = "rgb(" + r + "," + g + ",0)"
                ctx.fillRect(2, 6, fillWidth, height - 12)
            }

            Timer {
                interval: 500
                repeat: true
                running: true
                onTriggered: batteryCanvas.requestPaint()
            }
        }

        Text {
            text: root.voltage.toFixed(1) + " V"
            font.pixelSize: 16
            color: "white"
            Layout.alignment: Qt.AlignVCenter
        }
    }
}
