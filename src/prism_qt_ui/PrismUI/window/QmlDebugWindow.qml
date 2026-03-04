import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQml 2.12
import PrismCpp 1.0
import ".."

QmlDebugWindowCPP {
    id: win
    opacity: 0
    Component.onCompleted: {
        console.log("window created")
    }
    //modality: Qt.ApplicationModal


    onClosing:function(e){
        if(e.accepted)
        {
            console.log("window closing");
            CppUtility.setQmlOwnership(win)
            win.destroy()
        }
    }

    Component.onDestruction: {
        console.log("window destoryed")
    }



}
