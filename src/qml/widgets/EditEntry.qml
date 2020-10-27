import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import ".."



Column {
    id: column
    property alias name: name.text
    property alias value: value.text
    property alias placeholderText: value.placeholderText

    signal entryValueChanged(string value)

    Text {
        id: name
        width: parent.width
        text: "--name--"
        font: Constants.h4
    }

    TextField {
        id: value
        width: parent.width
        text: input
        placeholderText: "---"
        style: TextFieldStyle {
            textColor: Constants.edit_blue
            font: Constants.h5
            background: null            
        }
        onEditingFinished: {
            entryValueChanged(value.text)
        }
    }
}

