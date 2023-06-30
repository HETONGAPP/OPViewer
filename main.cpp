#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtOpcUa>
#include "opcuamodel.h"
#include "connectServer.h"

#include <QTreeView>
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    OpcUaModel opcUaModel;
    ConnectServer connectServer;
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("opcUaModel", &opcUaModel);
    engine.rootContext()->setContextProperty("connectServer", &connectServer);



    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return EXIT_FAILURE;

    const int exitCode = QCoreApplication::exec();

    return exitCode;
}


