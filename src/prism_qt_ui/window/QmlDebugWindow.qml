import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQml 2.12
import prismCpp 1.0
import ".."

QmlDebugWindowCPP {
    id: win
    //visible: true //应该在qml中设置好希望的宽和高后,再设置
    onFocusObjectChanged:function(obj) {
        //console.log(obj)
    }

    Component.onCompleted: {
        console.log("window created")
        //EventHubCpp.mainWindowClosed.connect(onMainWindowClosed);
    }
    modality: Qt.ApplicationModal
    function onMainWindowClosed() {
        win.close();
    }

    property var canCloseWindFunc: null

    onClosing:function(e){
        if(e.accepted)
        {
            console.log("window closing");
            CppUtility.setQmlOwnership(win)
            win.destroy()
            //EventHubCpp.mainWindowClosed.disconnect(onMainWindowClosed);
        }
    }

    Component.onDestruction: {
        console.log("window destoryed")
    }

    LiveLoader {
        id:loader
        active: !win.loadUrl.isEmpty
        source: active? CppUtility.transUrl( win.loadUrl):""
        anchors.fill: parent
    }


}
