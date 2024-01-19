import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0

Item {
    id: app

    Component.onCompleted: {
        FluApp.init(app)
//        FluApp.useSystemAppBar = true
//        FluTheme.darkMode = FluThemeType.Dark
        FluApp.routes = {
            "/":"qrc:/FluentPlayer/qml/main.qml",
        }
        FluApp.navigate("/")
    }
}
