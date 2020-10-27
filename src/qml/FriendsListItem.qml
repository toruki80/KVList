import QtQuick 2.4

FriendsListItemForm {
    signal activated()
    mousearea.onClicked: activated()
}
