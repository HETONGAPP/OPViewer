#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtOpcUa>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
//    engine.rootContext()->setContextProperty("uaBackend", &backend);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return EXIT_FAILURE;

    const int exitCode = QCoreApplication::exec();

    return exitCode;
}


