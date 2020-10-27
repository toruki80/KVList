import QtQuick 2.4
import QtQuick.Controls 2.14
import Insta 1.0

Item {
    id: root
    width: 400
    height: 800

    property var currentListItem: null
    property alias listview: listview

    Text {
        id: headline
        text: "Contacts"
        anchors.top: parent.top
        horizontalAlignment: Text.AlignHCenter
        textFormat: Text.AutoText
        font: Constants.h2
        color: "white"
        anchors.horizontalCenter: parent.horizontalCenter
    }


    ListView {
        id: listview
        anchors.left: parent.left
        anchors.right:parent.right
        anchors.bottom: parent.bottom
        anchors.top: headline.bottom
        anchors.topMargin: 15
        spacing: 4

        model: FriendsModel

        delegate: FriendsListItem {
            width: parent.width
            height: width  / 5

            onActivated: {
                currentListItem = model
            }
        }
    }


    Button {
        id: addButton
        icon.width: 96
        icon.height: 96
        display: AbstractButton.IconOnly
        icon.source: "icons/button_add.png"
        icon.color: Constants.white
        background: null

        anchors.right: parent.right
        anchors.bottom: parent.bottom

        onClicked: {
            listview.currentIndex = FriendsModel.addNewEntry();
            currentListItem = listview.currentItem.activated()
        }
    }
}
