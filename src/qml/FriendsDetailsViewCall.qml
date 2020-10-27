import QtQuick 2.4

FriendsDetailsViewCallForm {
    signal goToPage(string where)
    mouseArea.onClicked: { goToPage("info") }
}
