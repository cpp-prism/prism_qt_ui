import QtQuick.Layouts 1.12
import QtQuick 2.12
import QtQml 2.12
import QtQml.Models 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 1.4 as Q1
import QtQuick.Controls.Styles 1.4
import prismCpp 1.0
import ".."
import "../.."

Item {
    id:ro
    width: 800
    height: 400
    property string windowTitle: "窗口标题"
    property var viewModel: Window.window.viewModel
    onViewModelChanged: {
        if(viewModel)
            CppUtility.setCppOwnership(viewModel)
    }

    Component.onCompleted: {
        console.log("window content created")
        Window.window.width = width
        Window.window.height = height
        Window.window.x = Screen.width/2 - width/2
        Window.window.y = Screen.height/2 - height/2
        Window.window.title = windowTitle
        JsEx.delay(ro,100,function(){
            Window.window.visible = true
        })
    }
    Component.onDestruction: {
        console.log("window conetnt destroyed")
    }
}
