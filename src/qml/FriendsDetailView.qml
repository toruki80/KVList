import QtQuick 2.4

FriendsDetailViewForm {    
    signal goback()

    property bool requestRemove: false

    Component.onCompleted: {
        if(currentListItem.newlyCreatedEntry_ns) {
            currentListItem.newlyCreatedEntry_ns = false;
            viewInfo.goToPage("edit");
        }
    }

    viewInfo.onGoToPage: {
        switch(where) {
        case "back":
            goback();
            break;
        case "edit":
            backloader.source = "FriendsDetailViewEdit.qml?"+Math.random(); // the random trick forces the loader to reload
            flipable.flipped = true;
            break;

        case "mail":
            backloader.source = "FriendsDetailsViewCall.qml?"+Math.random();
            flipable.flipped = true;
            break;

        case "call":
            backloader.source = "FriendsDetailsViewCall.qml?"+Math.random();
            flipable.flipped = true;
            break;

        case "chat":
            backloader.source = "FriendsDetailsViewCall.qml?"+Math.random();
            flipable.flipped = true;
            break;

        case "video":
            backloader.source = "FriendsDetailsViewCall.qml?"+Math.random();
            flipable.flipped = true;
            break;
        }
    }

    Connections {
        target: backloader.item
        onGoToPage: {
            switch(where) {
            case "info":
                flipable.flipped = false;
                break;
            case "back":
                goback();
                break;
            case "remove":
                requestRemove = true;
                goback();
                break;
            }
        }
    }

    Component.onDestruction: {
        if(requestRemove) {
            var model = currentListItem.MODEL
            var ix = model.indexOf(currentListItem.ENTRY)
            model.deleteAt(ix);
            model.serialize();
        }
    }
}
