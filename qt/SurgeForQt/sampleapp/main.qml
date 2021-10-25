import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtMultimedia 5.15

Window {
    id: window
    width: 720
    height: 480
    visible: true
    title: qsTr("Hello World")

    TextEdit {
        id: urlInput
        height: 40
        text: viewModel.url
        anchors.left: parent.left
        anchors.right: startStreambutton.left
        anchors.top: parent.top
        font.pixelSize: 12
        verticalAlignment: Text.AlignVCenter
        anchors.rightMargin: 10
        anchors.leftMargin: 10
        anchors.topMargin: 0

        Binding {
            target: viewModel
            property: "url"
            value: urlInput.text
            when: urlInput.text !== viewModel.text
        }

        Binding {
            target: urlInput
            property: "text"
            value: viewModel.url
            when: urlInput.text !== viewModel.text
        }
    }

    Button {
        id: startStreambutton
        x: 340
        text: qsTr("Start Stream")
        anchors.right: stopStreamButton.left
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.rightMargin: 0
        onPressed: viewModel.onStartStreamButtonPressed()
    }

    Rectangle {
        id: blackBackground
        color: "#000000"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: startStreambutton.bottom
        anchors.bottom: parent.bottom
        anchors.rightMargin: 0
        anchors.leftMargin: 0
        anchors.bottomMargin: 0
        anchors.topMargin: 0
    }

    VideoOutput {
        id: videoOutput
        source: viewModel.player
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: startStreambutton.bottom
        anchors.bottom: parent.bottom
        anchors.rightMargin: 0
        anchors.leftMargin: 0
        anchors.bottomMargin: 0
        anchors.topMargin: 0
    }

    Button {
        id: stopStreamButton
        x: 440
        text: qsTr("Stop Stream")
        anchors.right: playButton.left
        anchors.top: parent.top
        anchors.rightMargin: 0
        anchors.topMargin: 0
        onPressed: viewModel.onStopStreamButtonPressed()
    }

    Button {
        id: playButton
        x: 540
        text: qsTr("Play")
        anchors.right: pauseButton.left
        anchors.top: parent.top
        anchors.rightMargin: 0
        anchors.topMargin: 0
        onPressed: viewModel.onPlayButtonPressed()
    }

    Button {
        id: pauseButton
        x: 590
        text: qsTr("Pause")
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 0
        anchors.topMargin: 0
        onPressed: viewModel.onPauseButtonPressed()
    }
}
