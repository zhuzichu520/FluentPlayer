import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import FluentUI

Item {
    id: app

    Component.onCompleted: {
        FluApp.init(app)
//        FluApp.useSystemAppBar = true
        FluTheme.darkMode = FluThemeType.Dark
        FluApp.routes = {
            "/":"qrc:/FluentPlayer/qml/main.qml",
        }
        FluApp.navigate("/")
    }
}
