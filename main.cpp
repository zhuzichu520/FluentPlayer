#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include "FluentPlayer.h"

int main(int argc, char *argv[])
{
    qputenv("QSG_RENDER_LOOP","threaded");
#ifdef WIN32
    qputenv("QT_QPA_PLATFORM","windows:darkmode=2");
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    qputenv("QT_QUICK_CONTROLS_STYLE","Basic");
#else
    qputenv("QT_QUICK_CONTROLS_STYLE","Default");
#endif
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
#endif
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    qmlRegisterType<FluentPlayer>("FluentPlayer", 1, 0, "FluentPlayer");
    const QUrl url(QStringLiteral("qrc:/App.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);
    return app.exec();
}
