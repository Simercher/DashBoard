import QtQuick 6.2
import QtQuick.Controls 6.5
import QtQuick.Layouts 6.2
import QtQuick.Window 6.2
import "components"

ApplicationWindow {
    id: window
    visible: true
    width: Screen.width
    height: Screen.height * 0.3
    minimumWidth: 800
    minimumHeight: 200
    title: "Simulated Robot"
    property bool robotEnabled: false
    property ListModel logHistory: ListModel {}
    property string consoleLogContent: ""

    function addLog(message) {
        logHistory.append({"text": message}) // 累積日誌
        // console.log("logHistory updated:", logHistory.count)
        // scrollTimer.restart() // 確保滾動條更新
    }

    // Component.onCompleted: {
    //     addLog("[System] Application started")
    // }

    Connections {
        target: window.logHistory
        function onCountChanged() {
            logListView.positionViewAtEnd()
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
                id: buttonColumn
                width: window.width * 0.1
                Layout.fillHeight: true
                anchors.margins: 8

                property int currentIndex: -1

                Repeater {
                    model: 5
                    Button {
                        id: toolButton
                        Layout.preferredHeight: 50
                        Layout.preferredWidth: 40

                        property bool toggled: false

                        background: Rectangle {
                            radius: 4
                            color: (index === buttonColumn.currentIndex) ? "#666" : "#444"
                        }

                        Image {
                            source: "qrc:/images/chauffer.png"
                            anchors.centerIn: parent
                            width: 24
                            height: 24
                        }

                        onClicked: buttonColumn.currentIndex = index
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
                            enabled: timeProvider.currentMode !== modelData && !window.robotEnabled
                            background: Rectangle {
                                color: timeProvider.currentMode === modelData ? "lightblue" : "white"
                                radius: 8
                                Behavior on color {
                                    ColorAnimation { duration: 20 }
                                }
                            }
                            onClicked: {
                                timeProvider.changingMode = true
                                timeProvider.startModeChange(modelData)
                                console.log("Mode changing to:", modelData)
                            }
                        }
                    }
                    Connections {
                        target: timeProvider
                        
                        function onModeChangeCompleted(newMode) {
                            timeProvider.changingMode = false  // Direct property assignment
                            console.log("Mode change completed:", newMode)
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
                                timeProvider.start()
                                addLog("[" + new Date().toLocaleTimeString() + "] Robot Enabled")
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
                                timeProvider.stop()
                                addLog("[" + new Date().toLocaleTimeString() + "] Robot Disabled")
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
                            id: timeDisplay
                            text: timeProvider.formattedTime || "00:00.0"
                            visible: true
                            color: "white"
                            font.pixelSize: 20
                            font.bold: true 
                            
                            // 簡化渲染設置，移除可能導致額外渲染負擔的屬性
                            renderType: Text.QtRendering  // 改用Qt渲染器
                            antialiasing: false
                            
                            // 禁用圖層化，圖層化在某些情況下會導致效能問題
                            layer.enabled: false
                            
                            Layout.alignment: Qt.AlignRight
                            Layout.preferredWidth: implicitWidth
                            horizontalAlignment: Text.AlignRight
                            
                            // 禁用文字動畫
                            Behavior on text {
                                enabled: false
                            }
                        }
                    }
                    Item { Layout.preferredHeight: 20 }

                    Text { text: "PC Battery"; color: "white"; font.pixelSize: 20 }
                    ProgressBar {
                        value: systemMonitor ? systemMonitor.batteryLevel : 0
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30
                        // Text {
                        //     anchors.centerIn: parent
                        //     text: Math.round(systemMonitor.batteryLevel * 100) + "%"
                        //     color: "white"
                        //     font.pixelSize: 14
                        // }
                    }
                    Text { text: "PC CPU %"; color: "white"; font.pixelSize: 20 }
                    ProgressBar {
                        value: systemMonitor ? systemMonitor.cpuUsage : 0
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30
                        // Text {
                        //     anchors.centerIn: parent
                        //     text: Math.round(systemMonitor.cpuUsage * 100) + "%"
                        //     color: "white"
                        //     font.pixelSize: 14
                        // }
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
                        text: timeProvider.currentMode + " " + (window.robotEnabled ? "Enabled" : "Disabled")
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
                                    text: model.text
                                    color: "white"
                                    font.pixelSize: 14
                                    wrapMode: Text.Wrap
                                    width: logListView.width
                                }
                                // 添加這些屬性來優化滾動行為
                                boundsMovement: Flickable.StopAtBounds
                                boundsBehavior: Flickable.StopAtBounds
                                // 添加平滑滾動
                                spacing: 2
                                smooth: true
                            }
                        }
                    }


                }
            }
        }
    }
}


// import QtQuick 2.15
// import QtQuick.Controls 2.15

// ApplicationWindow {
//     visible: true
//     width: 400
//     height: 200
//     title: qsTr("C++後台計時示範")

//     // 文字顯示區
//     Text {
//         id: timeText
//         anchors.centerIn: parent
//         text: timeProvider.elapsedMs + " ms"     // 直接讀取屬性
//         font.pointSize: 24
//     }

//     // 開始按鈕
//     Button {
//         id: startButton
//         text: qsTr("Start")
//         anchors.bottom: parent.bottom
//         anchors.left: parent.left
//         anchors.margins: 20
//         onClicked: timeProvider.start()
//     }

//     // 停止按鈕
//     Button {
//         id: stopButton
//         text: qsTr("Stop")
//         anchors.bottom: parent.bottom
//         anchors.right: parent.right
//         anchors.margins: 20
//         onClicked: timeProvider.stop()
//     }
// }
