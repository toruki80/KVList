pragma Singleton
import QtQuick 2.0

QtObject {
    id: themeing

    readonly property font overlay: Qt.font({
                                           family: "Skia",
                                           pixelSize: 48,
                                           bold: true
                                       })

    readonly property font h2: Qt.font({
                                           family: "Skia",
                                           pixelSize: 32,
                                           bold: true
                                       })

    readonly property font h4: Qt.font({
                                           family: "Skia",
                                           pixelSize: 22,
                                           bold: true
                                       })

    readonly property font h5: Qt.font({
                                           family: "Skia",
                                           pixelSize: 16
                                       })

    readonly property font h6: Qt.font({
                                           family: "Skia",
                                           pixelSize: 14
                                       })

    readonly property color orange: "#d97d54"
    readonly property color white: "#ffffff"
    readonly property color background_grey: "#edeae0"
    readonly property color edit_blue: "#09097f"
    readonly property color edit_activity_inactive: "grey"
    readonly property color edit_activity_active: "black"
    readonly property color item_grey: "#f5f4f4"
    readonly property int detailViewRadius: 4
}
