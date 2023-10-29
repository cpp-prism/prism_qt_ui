import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick 2.12
import QtQuick.Window 2.12
import QtGraphicalEffects 1.0
import prism_qt_ui 1.0
import prismCpp 1.0
import QtQml 2.12

Popup {
    id: pop
    width: 300
    height: 300
    closePolicy: Popup.CloseOnPressOutside
    padding: 0
    property real shadowThickness: 1
    default property alias content: contentPlaceholder.data

    background: Item {
        Item {
            id: bg_root
            anchors.fill: parent
            Item {
                anchors.fill: parent
                //anchors.margins: shadowThickness
                id: contentPlaceholder
            }
        }
        DropShadow {
            anchors.fill: parent
            source: bg_root
            horizontalOffset: 0
            verticalOffset: 0
            samples: 30
            spread: 0.4
            transparentBorder: true
            color: Style.gray10
        }
    }
}
