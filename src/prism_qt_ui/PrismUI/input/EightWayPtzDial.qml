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
    property string direction: "center"   // 方向
    property real strength: 0.0            // 0 ~ 1，距离比例
    property int directionCount: 8         // 方向数：4 或 8

    readonly property real cx: width / 2
    readonly property real cy: height / 2
    readonly property real maxR: ring_inner.width/2 - knob.width/2

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
        border.width: 1
        border.color: Style.lightgray30
        anchors.fill: parent
        radius: width / 2
        color: palette.bg
    }

    Rectangle {
        id:ring_inner
        border.width: 1
        border.color: Style.lightgray30
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
        color: (mouseArea.pressed || sectorBtns.pressedIdx >= 0) ? palette.active : palette.knob
        x: cx - width / 2
        y: cy - height / 2
        //scale: (mouseArea.pressed || sectorBtns.pressedIdx >= 0) ? 0.65 : 1.0

        /* 阴影 */
        Rectangle {
            anchors.fill: parent
            radius: parent.radius
            color: "#000"
            opacity: 0.25
            z: -1
            anchors.verticalCenterOffset: 3
        }

        //layer.enabled: mouseArea.pressed
        //layer.effect: MultiEffect {
        //    shadowEnabled: true
        //    //shadowBlurRadius: 24
        //    shadowColor: "white"
        //    shadowHorizontalOffset: 0
        //    shadowVerticalOffset: 0

        //}

        Behavior on x { SpringAnimation { spring: 4; damping: 0.45 } }
        Behavior on y { SpringAnimation { spring: 4; damping: 0.45 } }
        Behavior on scale { NumberAnimation { duration: 120 } }
        Behavior on color { ColorAnimation { duration: 120 } }
    }

    /* ===== 扇形按键 ===== */
    Item {
        id: sectorBtns
        anchors.fill: parent
        z: 2

        property int pressedIdx: -1

        readonly property var dirs: root.directionCount === 4
            ? ["right", "down", "left", "up"]
            : ["right", "down-right", "down", "down-left", "left", "up-left", "up", "up-right"]

        readonly property var angles: root.directionCount === 4
            ? [0, 90, 180, -90]
            : [0, 45, 90, 135, 180, -135, -90, -45]

        readonly property real sectorAngle: 360 / root.directionCount
        readonly property real innerR: root.width * 0.36
        readonly property real outerR: root.width * 0.5

        Canvas {
            id: sectorCanvas
            anchors.fill: parent

            onPaint: {
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)
                var cxp = width / 2
                var cyp = height / 2
                var ac = palette.active
                var kc = palette.knob
                var halfSector = sectorBtns.sectorAngle / 2

                for (var i = 0; i < root.directionCount; i++) {
                    var sa = (sectorBtns.angles[i] - halfSector) * Math.PI / 180
                    var ea = (sectorBtns.angles[i] + halfSector) * Math.PI / 180

                    ctx.beginPath()
                    ctx.arc(cxp, cyp, sectorBtns.outerR, sa, ea)
                    ctx.arc(cxp, cyp, sectorBtns.innerR, ea, sa, true)
                    ctx.closePath()

                    if (i === sectorBtns.pressedIdx) {
                        ctx.fillStyle = Qt.rgba(ac.r, ac.g, ac.b, 0.55)
                    } else {
                        ctx.fillStyle = Qt.rgba(kc.r, kc.g, kc.b, 0.10)
                    }
                    ctx.fill()

                    // 扇形分割线
                    ctx.beginPath()
                    ctx.moveTo(cxp + sectorBtns.innerR * Math.cos(sa),
                               cyp + sectorBtns.innerR * Math.sin(sa))
                    ctx.lineTo(cxp + sectorBtns.outerR * Math.cos(sa),
                               cyp + sectorBtns.outerR * Math.sin(sa))
                    ctx.strokeStyle = Qt.rgba(kc.r, kc.g, kc.b, 0.08)
                    ctx.lineWidth = 1
                    ctx.stroke()
                }
            }
        }

        Repeater {
            model: root.directionCount
            Text {
                required property int index
                property real midA: sectorBtns.angles[index] * Math.PI / 180
                property real midR: (sectorBtns.innerR + sectorBtns.outerR) / 2
                x: root.cx + midR * Math.cos(midA) - width / 2
                y: root.cy + midR * Math.sin(midA) - height / 2
                text: "\u25B2"
                font.pixelSize: (sectorBtns.outerR - sectorBtns.innerR) * 0.42
                color: index === sectorBtns.pressedIdx ? Material.foreground : Material.accent
                opacity: index === sectorBtns.pressedIdx ? 0.9 : 0.4
                rotation: sectorBtns.angles[index] + 90

                Behavior on opacity { NumberAnimation { duration: 100 } }
                Behavior on color   { ColorAnimation  { duration: 100 } }
            }
        }

        MouseArea {
            anchors.fill: parent

            function hitSector(mx, my) {
                var dx = mx - root.cx
                var dy = my - root.cy
                var dist = Math.sqrt(dx * dx + dy * dy)
                if (dist < sectorBtns.innerR || dist > sectorBtns.outerR)
                    return -1

                var deg = Math.atan2(dy, dx) * 180 / Math.PI
                var halfSector = sectorBtns.sectorAngle / 2

                for (var i = 0; i < root.directionCount; i++) {
                    var a = sectorBtns.angles[i]
                    var lo = a - halfSector
                    var hi = a + halfSector
                    if (a === 180) {
                        if (deg >= (180 - halfSector) || deg < -(180 - halfSector)) return i
                    } else {
                        if (deg >= lo && deg < hi) return i
                    }
                }
                return -1
            }

            onPressed: function(mouse) {
                var idx = hitSector(mouse.x, mouse.y)
                if (idx >= 0) {
                    sectorBtns.pressedIdx = idx
                    sectorCanvas.requestPaint()
                    root.strength = 1.0
                    root.direction = sectorBtns.dirs[idx]
                    var rad = sectorBtns.angles[idx] * Math.PI / 180
                    knob.x = root.cx + root.maxR * Math.cos(rad) - knob.width / 2
                    knob.y = root.cy + root.maxR * Math.sin(rad) - knob.height / 2
                } else {
                    mouse.accepted = false
                }
            }

            onReleased: {
                if (sectorBtns.pressedIdx >= 0) {
                    sectorBtns.pressedIdx = -1
                    sectorCanvas.requestPaint()
                    root.strength = 0.0
                    root.direction = "center"
                    knob.x = root.cx - knob.width / 2
                    knob.y = root.cy - knob.height / 2
                }
            }
        }
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
            strength = 0.0
            direction = "center"
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

            if (directionCount === 4) {
                // 4方向模式：每个方向覆盖90度
                if (deg >= -45 && deg < 45) return "right"
                if (deg >= 45 && deg < 135) return "down"
                if (deg >= 135 || deg < -135) return "left"
                if (deg >= -135 && deg < -45) return "up"
            } else {
                // 8方向模式：每个方向覆盖45度
                if (deg >= -22.5 && deg < 22.5) return "right"
                if (deg >= 22.5 && deg < 67.5) return "down-right"
                if (deg >= 67.5 && deg < 112.5) return "down"
                if (deg >= 112.5 && deg < 157.5) return "down-left"
                if (deg >= 157.5 || deg < -157.5) return "left"
                if (deg >= -157.5 && deg < -112.5) return "up-left"
                if (deg >= -112.5 && deg < -67.5) return "up"
                if (deg >= -67.5 && deg < -22.5) return "up-right"
            }

            return "center"
        }
    }
}
