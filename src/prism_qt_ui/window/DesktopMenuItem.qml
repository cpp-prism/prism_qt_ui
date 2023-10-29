import QtQml 2.15
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.4 as Q1
import QtQuick.Controls.Styles 1.3
import QtQuick.Controls.Styles 1.4
import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import "../theme"

Q1.MenuItem {
    property bool iconMipmap: false
    property real iconWidth: 20
    property real iconHeight: 20
    property real iconLM: 0
    property real iconRM: 10
    property color iconColor: Style.black50
    property real iconOpacity: 1.0
}
