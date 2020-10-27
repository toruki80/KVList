import QtQuick 2.4
import QtQuick.Controls 2.10
import QtQml 2.10
import Insta 1.0

Item {
    id: root
    width: 400
    height: 400

    property alias listdepth: stackView.listdepth

    onListdepthChanged: {
        background.x = listdepth * (root.width * 3) / 4
    }

    // background image that moves with content
    Item {
        anchors.fill: parent
        clip: true

        Image {
            id: background
            x: 0
            y: 0
            source: "icons/background.jpg"

            Behavior on x {

                NumberAnimation {
                    //This specifies how long the animation takes
                    duration: 500
                    //This selects an easing curve to interpolate with, the default is Easing.Linear
                    easing.type: Easing.InOutQuad
                }
            }
        }
    }


    StackView {
        id: stackView
        anchors.fill: parent
        anchors.margins: 10
        property int listdepth: 0

        initialItem: FriendsList {
            id: list
            width: stackView.width
            height: stackView.height

            onCurrentListItemChanged: {
                if(currentListItem) {
                    stackView.push(listDetails,
                                   {currentListItem: list.currentListItem})
                    stackView.listdepth --;
                }
            }
        }
    }

    Component {
        id: listDetails
        FriendsDetailView {
            width: stackView.width
            height: stackView.height
            onGoback: {
                stackView.pop();
                stackView.listdepth ++;
            }
        }
    }
}
