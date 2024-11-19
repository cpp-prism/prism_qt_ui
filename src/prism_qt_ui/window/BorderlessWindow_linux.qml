import QtQuick 2.0
import QtQuick.Window 2.12
import prismCpp 1.0
import QtQuick.Layouts 1.12
import prism_qt_ui 1.0
import dv.viewmodels 1.0
import QtQuick.Shapes 1.15

Window {
    id: window
    visible: true
    title: qsTr("prism")
    color: "white"
    //flags: Qt.Window | Qt.WindowMaximizeButtonHint | Qt.WindowMinimizeButtonHint
    //       | Qt.WindowCloseButtonHint | Qt.WindowSystemMenuHint |Qt.FramelessWindowHint
    minimumWidth: 1030
    minimumHeight: 670


    property int frameBorderWidth: 0
    property color frameBorderColor: "lightgray"
    property  alias showReloadButton: window_rootLoader.showButton
    Shape {
        id: mask
        visible:frameBorderWidth!==0 && window.visibility === Window.Windowed
        containsMode: Shape.FillContains
        anchors.fill: parent
        ShapePath {
            fillColor: frameBorderColor
            strokeColor: frameBorderColor
            strokeWidth: 0
            startX: 0
            startY: 0
            PathLine {
                x: 0
                y: window.height
            }
            PathLine {
                x: window.width
                y: window.height
            }
            PathLine {
                x: window.width
                y: 0
            }
            PathLine {
                x: 0
                y: 0
            }
            PathLine {
                x: frameBorderWidth
                y: frameBorderWidth
            }

            PathLine {
                x: frameBorderWidth
                y: window.height - frameBorderWidth
            }
            PathLine {
                x: window.width - frameBorderWidth
                y: window.height - frameBorderWidth
            }
            PathLine {
                x: window.width - frameBorderWidth
                y: frameBorderWidth
            }
            PathLine {
                x: frameBorderWidth
                y: frameBorderWidth
            }
        }
    }
    Rectangle{
        color: window.color
        anchors.fill: parent
        anchors.margins: window.visibility === Window.Windowed? frameBorderWidth:0
        LiveLoader {
            id: window_rootLoader
            anchors.fill: parent
            source: contentUrl
        }
    }
}
