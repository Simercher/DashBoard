#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQmlContext>
#include "SystemMonitor.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("Material");

    QQmlApplicationEngine engine;
    SystemMonitor* systemMonitor = new SystemMonitor(&engine);
    engine.rootContext()->setContextProperty("systemMonitor", systemMonitor);
    engine.loadFromModule("DriverStation", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
