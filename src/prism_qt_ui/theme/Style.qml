pragma Singleton

import QtQml 2.12
import QtQml 2.15
import QtQuick 2.15
import ".."

QtObject {
    id: style
    //property var eventhub: EventsHub
    //onEventhubChanged: {
    //    eventhub.prismThemeChanged.connect(onThemeChanged)
    //    eventhub.prismEvent(EventsHub.InitStyle, null)
    //}
    function onThemeChanged(color) {
        style.color100 = JsEx.setColorAlpha(color, 1.0)
        style.color90 = JsEx.setColorAlpha(color, 0.9)
        style.color80 = JsEx.setColorAlpha(color, 0.8)
        style.color70 = JsEx.setColorAlpha(color, 0.7)
        style.color60 = JsEx.setColorAlpha(color, 0.6)
        style.color50 = JsEx.setColorAlpha(color, 0.5)
        style.color40 = JsEx.setColorAlpha(color, 0.4)
        style.color30 = JsEx.setColorAlpha(color, 0.3)
        style.color20 = JsEx.setColorAlpha(color, 0.2)
        style.color10 = JsEx.setColorAlpha(color, 0.1)
    }
    property font h1: JsEx.getFont(32)
    property font h2: JsEx.getFont(24)
    property font h3: JsEx.getFont(18)
    property font h4: JsEx.getFont(16)
    property font h5: JsEx.getFont(14)
    property font h6: JsEx.getFont(13)
    property font h7: JsEx.getFont(12)

    property color lightOrDark
    property color light: "white"
    property color dark: "black"

    property color text_selectionColor: "white"
    property color text_selectionBgColor: "#338fff"

    property color color100
    property color color90
    property color color80
    property color color70
    property color color60
    property color color50
    property color color40
    property color color30
    property color color20
    property color color10

    property color white: "white"
    property color white100: JsEx.setColorAlpha(white, 1.0)
    property color white90: JsEx.setColorAlpha(white, 0.9)
    property color white80: JsEx.setColorAlpha(white, 0.8)
    property color white70: JsEx.setColorAlpha(white, 0.7)
    property color white60: JsEx.setColorAlpha(white, 0.6)
    property color white50: JsEx.setColorAlpha(white, 0.5)
    property color white40: JsEx.setColorAlpha(white, 0.4)
    property color white30: JsEx.setColorAlpha(white, 0.3)
    property color white20: JsEx.setColorAlpha(white, 0.2)
    property color white10: JsEx.setColorAlpha(white, 0.1)

    property color lightblue: "lightblue"
    property color lightblue100: JsEx.setColorAlpha(lightblue, 1.0)
    property color lightblue90: JsEx.setColorAlpha(lightblue, 0.9)
    property color lightblue80: JsEx.setColorAlpha(lightblue, 0.8)
    property color lightblue70: JsEx.setColorAlpha(lightblue, 0.7)
    property color lightblue60: JsEx.setColorAlpha(lightblue, 0.6)
    property color lightblue50: JsEx.setColorAlpha(lightblue, 0.5)
    property color lightblue40: JsEx.setColorAlpha(lightblue, 0.4)
    property color lightblue30: JsEx.setColorAlpha(lightblue, 0.3)
    property color lightblue20: JsEx.setColorAlpha(lightblue, 0.2)
    property color lightblue10: JsEx.setColorAlpha(lightblue, 0.1)

    property color blue: "blue"
    property color blue100: JsEx.setColorAlpha(blue, 1.0)
    property color blue90: JsEx.setColorAlpha(blue, 0.9)
    property color blue80: JsEx.setColorAlpha(blue, 0.8)
    property color blue70: JsEx.setColorAlpha(blue, 0.7)
    property color blue60: JsEx.setColorAlpha(blue, 0.6)
    property color blue50: JsEx.setColorAlpha(blue, 0.5)
    property color blue40: JsEx.setColorAlpha(blue, 0.4)
    property color blue30: JsEx.setColorAlpha(blue, 0.3)
    property color blue20: JsEx.setColorAlpha(blue, 0.2)
    property color blue10: JsEx.setColorAlpha(blue, 0.1)

    property color red: "red"
    property color red100: JsEx.setColorAlpha(red, 1.0)
    property color red90: JsEx.setColorAlpha(red, 0.9)
    property color red80: JsEx.setColorAlpha(red, 0.8)
    property color red70: JsEx.setColorAlpha(red, 0.7)
    property color red60: JsEx.setColorAlpha(red, 0.6)
    property color red50: JsEx.setColorAlpha(red, 0.5)
    property color red40: JsEx.setColorAlpha(red, 0.4)
    property color red30: JsEx.setColorAlpha(red, 0.3)
    property color red20: JsEx.setColorAlpha(red, 0.2)
    property color red10: JsEx.setColorAlpha(red, 0.1)

    property color lightgray: "lightgray"
    property color lightgray100: JsEx.setColorAlpha(lightgray, 1.0)
    property color lightgray90: JsEx.setColorAlpha(lightgray, 0.9)
    property color lightgray80: JsEx.setColorAlpha(lightgray, 0.8)
    property color lightgray70: JsEx.setColorAlpha(lightgray, 0.7)
    property color lightgray60: JsEx.setColorAlpha(lightgray, 0.6)
    property color lightgray50: JsEx.setColorAlpha(lightgray, 0.5)
    property color lightgray40: JsEx.setColorAlpha(lightgray, 0.4)
    property color lightgray30: JsEx.setColorAlpha(lightgray, 0.3)
    property color lightgray20: JsEx.setColorAlpha(lightgray, 0.2)
    property color lightgray10: JsEx.setColorAlpha(lightgray, 0.1)

    property color gray: "gray"
    property color gray100: JsEx.setColorAlpha(gray, 1.0)
    property color gray90: JsEx.setColorAlpha(gray, 0.9)
    property color gray80: JsEx.setColorAlpha(gray, 0.8)
    property color gray70: JsEx.setColorAlpha(gray, 0.7)
    property color gray60: JsEx.setColorAlpha(gray, 0.6)
    property color gray50: JsEx.setColorAlpha(gray, 0.5)
    property color gray40: JsEx.setColorAlpha(gray, 0.4)
    property color gray30: JsEx.setColorAlpha(gray, 0.3)
    property color gray20: JsEx.setColorAlpha(gray, 0.2)
    property color gray10: JsEx.setColorAlpha(gray, 0.1)

    property color black: "black"
    property color black100: JsEx.setColorAlpha(black, 1.0)
    property color black90: JsEx.setColorAlpha(black, 0.9)
    property color black80: JsEx.setColorAlpha(black, 0.8)
    property color black70: JsEx.setColorAlpha(black, 0.7)
    property color black60: JsEx.setColorAlpha(black, 0.6)
    property color black50: JsEx.setColorAlpha(black, 0.5)
    property color black40: JsEx.setColorAlpha(black, 0.4)
    property color black30: JsEx.setColorAlpha(black, 0.3)
    property color black20: JsEx.setColorAlpha(black, 0.2)
    property color black10: JsEx.setColorAlpha(black, 0.1)
}
