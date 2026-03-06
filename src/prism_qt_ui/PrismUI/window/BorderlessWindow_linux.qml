import QtQuick 2.15
import QtQuick.Window 2.12
import PrismCpp 1.0
import QtQuick.Layouts 1.12
import PrismUI 1.0
import QtQuick.Shapes 1.15
import QtQuick.Controls.Material
import QtQuick.Effects

QmlDebugWindow {
    id: window



    property bool showbtn_maxRes : true
    property int titleHeight : 30
    property int titleBarBtnWidth :80

    property string svgPath: "qrc:/rs_common/assets/icon.svg"

    property int frameBorderWidth: 0
    property color frameBorderColor: "lightgray"
    property color titleColor: "#3F51B5"
    property  alias showReloadButton: window_rootLoader.showButton
    property  alias titleReloader: title_rootLoader

    visible: true
    title: qsTr("prism")
    color: "white"
    flags: Qt.Window | Qt.WindowMaximizeButtonHint | Qt.WindowMinimizeButtonHint | Qt.WindowCloseButtonHint | Qt.WindowSystemMenuHint |Qt.FramelessWindowHint
    //minimumWidth: 1030
    //minimumHeight: 670

    Component.onCompleted: {
        opacity= 0
        JsEx.delay(window,500,function(){
            CppUtility.centerQmlWindow2cursorScreen(win)
            opacity= 1
        })
    }

    property string titleUrl
    property Component titleComponent:
        Rectangle {
        color:titleColor
        MouseArea{
            anchors.fill: parent
            onPressed:  function(mouse){
                if(mouse.button === Qt.LeftButton)
                    window.startSystemMove()

            }
            onDoubleClicked: {
                if(!showbtn_maxRes )
                    return
                if ( window.visibility === Window.Maximized)
                    window.showNormal()
                else
                    window.showMaximized()
            }
        }
        RowLayout{
            anchors.fill: parent
            spacing: 5
            SvgIcon
            {
                Layout.preferredWidth:  25
                Layout.preferredHeight:  25
                visible: svgPath ==""?false:true
                color: "transparent"
                Layout.alignment: Qt.AlignVCenter
                Layout.leftMargin: 5
                mipmap: true
                svgPath:"qrc:/rs_common/assets/icon.svg"
            }

            Text {
                Layout.leftMargin:  svgPath =="" ? 5 :0
                color:Style.white
                text: title
                font.pixelSize: 18
                z:9999
            }
            Item {
                Layout.fillWidth: true
            }
        }

        Rectangle{
            anchors.right: parent.right
            height: titleHeight
            width: titleBarBtnWidth
            color: "#3F51B5"
            //color: Material.Grey
            RowLayout{
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                spacing: 0
                //最小化按钮
                IconButton {
                    Layout.preferredHeight: 13
                    Layout.preferredWidth: 13
                    Layout.rightMargin: 16
                    color: Style.lightblue100
                    hoveredColor: Style.white
                    icon: CppUtility.transUrl("qrc:/prism_qt_ui/PrismUI/svg/windowMinimize.svg")
                    onClicked: function (mouse) {
                        window.showMinimized()
                    }
                }
                //最大化/还原
                IconButton {
                    id: btn_maxRes
                    color: Style.lightblue100
                    hoveredColor: Style.white
                    visible: showbtn_maxRes
                    states: [
                        State {
                            name: "maximized"
                            when: window.visibility === Window.Maximized
                            PropertyChanges {
                                target: btn_maxRes
                                Layout.rightMargin: 12
                                Layout.preferredHeight: 18
                                Layout.preferredWidth: 17
                                icon: CppUtility.transUrl("qrc:/prism_qt_ui/PrismUI/svg/windowRestore.svg")
                            }
                        },
                        State {
                            when: window.visibility !== Window.Maximized
                            name: "normal"
                            PropertyChanges {
                                target: btn_maxRes
                                Layout.rightMargin: 12
                                Layout.preferredHeight: 10
                                Layout.preferredWidth: 13
                                icon: CppUtility.transUrl("qrc:/prism_qt_ui/PrismUI/svg/windowMaximize.svg")
                            }
                        }
                    ]
                    onClicked: function (mouse) {
                        if (window.visibility === Window.Maximized)
                            window.showNormal()
                        else
                            window.showMaximized()
                    }
                }

                //关闭
                IconButton {
                    Layout.preferredHeight: 20
                    Layout.preferredWidth: 20
                    Layout.rightMargin: 10
                    color: Style.lightblue100
                    hoveredColor: Style.white
                    icon: CppUtility.transUrl("qrc:/prism_qt_ui/PrismUI/svg/windowClose.svg")
                    onClicked: function (mouse) {
                        window.close()
                    }
                }

            }
        }
    }

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
        ColumnLayout {
            anchors.fill: parent
            spacing: 0


            LiveLoader {
                id:title_rootLoader
                Layout.preferredHeight: titleHeight
                Layout.fillWidth: true
                showButton: false
                source: titleUrl
                sourceComponent: titleUrl?null: titleComponent
            }

            LiveLoader{
                id:window_rootLoader
                Layout.fillHeight: true
                Layout.fillWidth: true
                clip: true
                source: loadUrl
                //source: CppUtility.transUrl("qrc:/rs_common/views/layouts/body.qml")
            }


        }
    }

    MouseArea{
        id:ma_borderframe
        anchors.fill: parent
        enabled: window.visibility == Window.Windowed
        containmentMask: mask
        hoverEnabled: true
        property var edge;
        function hitTest(x, y) {
            const bw = frameBorderWidth
            const w = width
            const h = height

            const left   = x <= bw
            const right  = x >= (w - bw)
            const top    = y <= bw
            const bottom = y >= (h - bw)

            var tmp =0
            if(top)
                tmp |= Qt.TopEdge
            if(left)
                tmp |= Qt.LeftEdge
            if(right)
                tmp |= Qt.RightEdge
            if(bottom)
                tmp |= Qt.BottomEdge
            edge = tmp

        }
        onPositionChanged: {
            hitTest(mouseX, mouseY)

            const left   = edge & Qt.LeftEdge
            const right  = edge & Qt.RightEdge
            const top    = edge & Qt.TopEdge
            const bottom = edge & Qt.BottomEdge
            if((top && left) || (bottom && right))
            {
                CppUtility.restoreCursor()
                CppUtility.setCursor(Qt.SizeFDiagCursor)
                return
            }
            else if(( top && right)||( bottom && left))
            {
                CppUtility.restoreCursor()
                CppUtility.setCursor(Qt.SizeBDiagCursor)
                return
            }
            else if(left || right)
            {
                CppUtility.restoreCursor()
                CppUtility.setCursor(Qt.SizeHorCursor)
                return
            }
            else if(top || bottom)
            {
                CppUtility.restoreCursor()
                CppUtility.setCursor(Qt.SizeVerCursor)
                return
            }
            else
            {
                CppUtility.restoreCursor()
                CppUtility.setCursor(Qt.ArrowCursor)
            }
        }
        onEntered:  hitTest(mouseX, mouseY)
        onExited: {
            hitTest(mouseX, mouseY)
            CppUtility.restoreCursor()
        }
        onPressed: {
            hitTest(mouseX,mouseY)
            {
                window.startSystemResize(edge)
                CppUtility.restoreCursor()
            }
        }
    }
}
