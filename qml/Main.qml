import QtQuick 6.2
import QtQuick.Controls 6.5
import QtQuick.Layouts 6.2
import QtQuick.Window 6.2
import DriverStation 1.0
import "components"

ApplicationWindow {
    id: window
    visible: true
    width: Screen.width
    height: Screen.height * 0.3
    minimumWidth: 800
    minimumHeight: 200
    title: "Simulated Robot"
    property string currentMode: "TeleOperated"
    property bool robotEnabled: false
    property bool changingMode: false
    property string targetMode: ""  // 新增目標模式屬性
    property ListModel logHistory: ListModel {}
    property string consoleLogContent: ""
    property int elapsedTimeMs: 0

    function addLog(message) {
        logHistory.append({"text": message}) // 累積日誌
        console.log("logHistory updated:", logHistory.count)
        scrollTimer.restart() // 確保滾動條更新
    }

    // Component.onCompleted: {
    //     addLog("[System] Application started")
    // }

    Timer {
        id: elapsedTimer
        interval: 100  // 每0.1秒更新一次
        repeat: true
        running: window.robotEnabled
        onTriggered: {
            elapsedTimeMs += 100
        }
    }

    Timer {
        id: scrollTimer
        interval: 200
        repeat: false
        onTriggered: {
            logListView.positionViewAtEnd()
        }
    }

    Timer {
        id: modeChangeTimer
        interval: 200
        repeat: false
        onTriggered: {
            window.currentMode = targetMode
            changingMode = false
            var timestamp = new Date().toLocaleTimeString()
            var logMessage = "[" + timestamp + "] Mode changed to: " + targetMode + "\n"
            addLog(logMessage)
            console.log("Mode changed to:", targetMode)
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#2b2b2b"

        RowLayout {
            anchors.fill: parent
            spacing: 0

            // 左邊工具欄
            ColumnLayout {
                width: window.width * 0.1
                Layout.fillHeight: true
                anchors.margins: 8

                Repeater {
                    model: 5
                    Rectangle {
                        Layout.preferredHeight: 50
                        Layout.preferredWidth: 40
                        radius: 4
                        color: "#444"
                    }
                }
            }

            // 中央控制區
            Rectangle {
                width: window.width * 0.15
                color: "#333"
                Layout.preferredHeight: parent.height
                RowLayout {
                    anchors.fill: parent
                    spacing: 0

                }

                // 你的中央控制區內容在這裡放進來
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 8
                    anchors.margins: 12

                    // 四個模式按鈕
                    Repeater {
                        model: ["TeleOperated", "Autonomous", "Practice", "Test"]
                        Button {
                            text: modelData
                            Layout.fillWidth: true
                            font.pixelSize: 14
                            enabled: !changingMode && !window.robotEnabled
                            background: Rectangle {
                                color: window.currentMode === modelData ? "lightblue" : "white"
                                radius: 8
                                Behavior on color {
                                    ColorAnimation { duration: 20 }
                                }
                            }
                            onClicked: {
                                changingMode = true
                                // window.currentMode = modelData
                                targetMode = modelData
                                // var timestamp = new Date().toLocaleTimeString()
                                // window.consoleLogContent += "[" + timestamp + "] Mode changed to: " + modelData + "\n"
                                modeChangeTimer.restart()
                                console.log("Mode changed to:", modelData)
                            }
                        }
                    }

                    // Enable / Disable 按鈕
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Button {
                            text: "Enable"
                            Layout.fillWidth: true
                            background: Rectangle {
                                color: "green"
                                radius: 8
                            }
                            font.pixelSize: 14
                            font.bold: true
                            enabled: !window.robotEnabled
                            onClicked: {
                                window.robotEnabled = true
                                elapsedTimeMs = 0
                                var timestamp = new Date().toLocaleTimeString()
                                var logMessage = "[" + timestamp + "] Robot Enabled\n"
                                addLog(logMessage)
                                console.log("Robot Enabled")
                            }

                        }
                        Button {
                            text: "Disable"
                            Layout.fillWidth: true
                            background: Rectangle {
                                color: "red"
                                radius: 8
                            }
                            font.pixelSize: 14
                            font.bold: true
                            enabled: window.robotEnabled
                            onClicked: {
                                window.robotEnabled = false
                                var timestamp = new Date().toLocaleTimeString()
                                var logMessage = "[" + timestamp + "] Robot Disabled\n"
                                addLog(logMessage)
                                console.log("Robot Disabled")
                            }
                        }
                    }
                }
            }

            Rectangle {
                width: window.width * 0.15
                color: "#333"
                Layout.preferredHeight: parent.height
                // 中間狀態區塊
                ColumnLayout {
                    id: statusInfo
                    // anchors.top: controlButtons.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    // anchors.bottom: bottomButtons.top
                    anchors.margins: 12
                    spacing: 10
                    // 增加往下位移空間
                    Item { Layout.preferredHeight: 20 }

                    RowLayout {
                        Layout.fillWidth: true
                        Text { text: "Elapsed Time"; color: "white"; font.pixelSize: 22 }
                        Item { Layout.fillWidth: true }
                        Text { 
                            text: {
                                let minutes = Math.floor(elapsedTimeMs / 60000)
                                let seconds = Math.floor((elapsedTimeMs % 60000) / 1000)
                                let tenths = Math.floor((elapsedTimeMs % 1000) / 100)
                                return minutes + ":" + 
                                    (seconds < 10 ? "0" : "") + seconds + "." + 
                                    tenths
                            }
                            color: "white"
                            font.pixelSize: 20
                            font.bold: true 
                        }
                    }
                    Item { Layout.preferredHeight: 20 }

                    Text { text: "PC Battery"; color: "white"; font.pixelSize: 20 }
                    ProgressBar {
                        value: 0.7
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30
                    }

                    Text { text: "PC CPU %"; color: "white"; font.pixelSize: 20 }
                    ProgressBar {
                        value: 0.5
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30
                    }
                }
            }

            // 狀態欄
            Rectangle {
                width: window.width * 0.15
                color: "#222"
                Layout.preferredHeight: parent.height
                // 內容
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 10

                    Text {
                        text: "Simulated Robot"
                        font.pixelSize: 20
                        font.bold: true
                        color: "skyblue"
                    }
                    BatteryIndicator {
                        voltage: 5
                    }



                    ColumnLayout {
                        spacing: 4
                        Repeater {
                            model: ["Communications", "Robot Code", "Joysticks"]
                            delegate: RowLayout {
                                spacing: 8
                                Rectangle {
                                    width: 12; height: 12; radius: 6
                                    color: "limegreen"
                                    // anchors.verticalCenter: parent.verticalCenter
                                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                                }
                                Text {
                                    text: modelData
                                    color: "white"
                                    font.pixelSize: 14
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                        }
                    }


                    Text {
                        text: window.currentMode + " " + (window.robotEnabled ? "Enabled" : "Disabled")
                        font.pixelSize: 16
                        font.bold: true
                        color: window.robotEnabled ? "lightgreen" : "orange"
                    }
                }

            }
            // Console log 區塊
            Rectangle {
                Layout.fillWidth: true
                color: "#111"
                Layout.preferredHeight: parent.height

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 8

                    Text {
                        text: "Console Log"
                        font.pixelSize: 16
                        font.bold: true
                        color: "white"
                    }

                    Rectangle {
                        color: "#222"
                        radius: 6
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        ScrollView {
                            id: scrollView
                            anchors.fill: parent
                            clip: true
                            ScrollBar.vertical.policy: ScrollBar.AlwaysOn

                            ListView {
                                id: logListView
                                model: window.logHistory
                                delegate: Text {
                                    text: model.text || text
                                    color: "white"
                                    font.pixelSize: 14
                                    wrapMode: Text.Wrap
                                }
                            }
                        }
                    }


                }
            }
        }
    }
}
