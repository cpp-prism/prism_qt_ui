import QtQuick

Canvas {
    property color color:"white"
    property int radius: 10
    property bool mirrorX: false
    property bool mirrorY: false
    onPaint: {
        const ctx = getContext("2d")
        ctx.reset()
        ctx.clearRect(0, 0, width, height)

        ctx.fillStyle = color
        ctx.fillRect(0, 0, width, height)

        ctx.globalCompositeOperation = "destination-out"

        const r = Math.min(radius, width / 2, height / 2)

        ctx.beginPath()
        ctx.moveTo(r, 0)
        ctx.lineTo(width - r, 0)
        ctx.arc(width - r, r, r, -Math.PI / 2, 0)

        ctx.lineTo(width, height - r)
        ctx.arc(width - r, height - r, r, 0, Math.PI / 2)

        ctx.lineTo(r, height)
        ctx.arc(r, height - r, r, Math.PI / 2, Math.PI)

        ctx.lineTo(0, r)
        ctx.arc(r, r, r, Math.PI, Math.PI * 1.5)

        ctx.closePath()
        ctx.fill()

        ctx.globalCompositeOperation = "source-over"
    }

}
