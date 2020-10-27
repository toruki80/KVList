import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12


ApplicationWindow {
    id: rootWindow
    width: 400
    height: 800
    visible: true
    title: qsTr("")


    PageFriends {
        anchors.fill: parent
    }


}
