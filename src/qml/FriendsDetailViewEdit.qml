import QtQuick 2.4
import QtQuick.Dialogs 1.3

FriendsDetailViewEditForm {

    signal goToPage(string where)

    saveButton.onClicked: {
        var cnt = currentListItem.ENTRY.applyShadowedChanges();
        cnt += currentListItem.activitiesAll.applyAllShadowedChanges();
        if(cnt > 0)
            currentListItem.MODEL.serialize();
        goToPage("info");
    }
    cancelButton.onClicked: {
        currentListItem.ENTRY.revertShadowedChanges();
        currentListItem.activitiesAll.revertAllShadowedChanges();
        goToPage("info");
    }
    deleteButton.onClicked: {
        goToPage("remove");
    }

    logoArea.onClicked: {
        fileDialog.open()
    }


    FileDialog {
        id: fileDialog
        modality: Qt.WindowModal
        title: "Please choose a image"
        folder: shortcuts.home
        nameFilters: ["Image File (*.png *.jpg *.jpeg *.bmp)"]

        onAccepted: currentListItem.logoURL_shadowed = fileDialog.fileUrl
    }
}
