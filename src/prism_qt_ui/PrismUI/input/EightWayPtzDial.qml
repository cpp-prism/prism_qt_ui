import QtQuick
import QtQuick.Controls
import QtQml
import QtQuick.Layouts
import PrismUI 1.0
import PrismCpp 1.0
import Rbs.Viewmodels 1.0
import QtQuick.Controls.Material
import QtQuick.Effects

Item {
    id: root
    width: 220
    height: 220

    /* ===== 对外状态 ===== */
    property string direction: "center"   // 8方向
    property real strength: 0.0            // 0 ~ 1，距离比例

    readonly property real cx: width / 2
    readonly property real cy: height / 2
    readonly property real maxR: width * 0.22

    /* ===== Material 配色 ===== */
    QtObject {
        id: palette
        readonly property color bg: Style.gray50
        readonly property color ring: Style.gray30
        readonly property color active: root.Material.accent
        readonly property color knob: root.Material.background
    }

    /* ===== 外圆 ===== */
    Rectangle {
        anchors.fill: parent
        radius: width / 2
        color: palette.bg
    }

    Rectangle {
        width: parent.width * 0.72
        height: width
        radius: width / 2
        anchors.centerIn: parent
        color: palette.ring
    }


    /* ===== 中心小球 ===== */
    Rectangle {
        id: knob
        width: 36
        height: 36
        radius: width / 2
        color: mouseArea.pressed ?  palette.active : palette.knob
        x: cx - width / 2
        y: cy - height / 2
        scale: mouseArea.pressed ? 0.65 : 1.0

        /* 阴影 */
        Rectangle {
            anchors.fill: parent
            radius: parent.radius
            color: "#000"
            opacity: 0.25
            z: -1
            anchors.verticalCenterOffset: 3
        }

        layer.enabled: mouseArea.pressed
        layer.effect: MultiEffect {
            shadowEnabled: true
            //shadowBlurRadius: 24
            shadowColor: "white"
            shadowHorizontalOffset: 0
            shadowVerticalOffset: 0

        }

        Behavior on x { SpringAnimation { spring: 4; damping: 0.45 } }
        Behavior on y { SpringAnimation { spring: 4; damping: 0.45 } }
        Behavior on scale { NumberAnimation { duration: 120 } }
        Behavior on color { ColorAnimation { duration: 120 } }
    }

    /* ===== 交互 ===== */
    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onPressed: update(mouseX, mouseY)
        onPositionChanged: if (pressed) update(mouseX, mouseY)
        onReleased: reset()

        function reset() {
            knob.x = cx - knob.width / 2
            knob.y = cy - knob.height / 2
            direction = "center"
            strength = 0.0
        }

        function update(x, y) {
            const dx = x - cx
            const dy = y - cy
            const dist = Math.sqrt(dx * dx + dy * dy)

            const r = Math.min(dist, maxR)
            const k = dist > 0 ? r / dist : 0

            knob.x = cx + dx * k - knob.width / 2
            knob.y = cy + dy * k - knob.height / 2

            strength = r / maxR

            if (dist < 10) {
                direction = "center"
                return
            }

            direction = angleToDir(Math.atan2(dy, dx))
        }

        function angleToDir(a) {
            const deg = a * 180 / Math.PI

            if (deg >= -22.5 && deg < 22.5) return "right"
            if (deg >= 22.5 && deg < 67.5) return "down-right"
            if (deg >= 67.5 && deg < 112.5) return "down"
            if (deg >= 112.5 && deg < 157.5) return "down-left"
            if (deg >= 157.5 || deg < -157.5) return "left"
            if (deg >= -157.5 && deg < -112.5) return "up-left"
            if (deg >= -112.5 && deg < -67.5) return "up"
            if (deg >= -67.5 && deg < -22.5) return "up-right"

            return "center"
        }
    }
}
