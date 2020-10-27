import QtQuick 2.0
import QtQuick.Controls 2.12
import Insta 1.0
import ".."

Button {
    id: control
    text: qsTr("Button")
    display: AbstractButton.TextOnly
    contentItem: Label {
        text: control.text
        font: Constants.h4
        color: Constants.orange
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    background: null
}
