#include "connectServer.h"
#include <QMessageBox>
#include <QOpcUaErrorState>
#include <QtQml>
#include "opcuamodel.h"

ConnectServer::ConnectServer(QObject *parent)
    : QObject{parent}
    , mOpcUaModel(new OpcUaModel(this))
    , mOpcUaProvider(new QOpcUaProvider(this))
    , m_sucs(false)
{
    qmlRegisterType<OpcUaModel>("ConnectServer", 1, 0, "ConnectServer");

}

ConnectServer::~ConnectServer()
{

}

void ConnectServer::connectToServer()
{


    m_endpoint = mEndpointList[0];
    mOpcUaClient = mOpcUaProvider->createClient("open62541");
    if (!mOpcUaClient) {
        const QString message(tr("Connecting to the given sever failed. See the log for details."));
        qDebug()<<"Failed to connect to server";
        return;
    }
    connect(mOpcUaClient, &QOpcUaClient::endpointsRequestFinished, this, &ConnectServer::getEndpointsComplete);
    connect(mOpcUaClient, &QOpcUaClient::connected, this, &ConnectServer::clientConnected);

    mOpcUaClient->connectToEndpoint(m_endpoint);

}


void ConnectServer::getEndpoint()
{
    const QString serverUrl ="opc.tcp://localhost:43344";
    mOpcUaClient = mOpcUaProvider->createClient("open62541");
    if (!mOpcUaClient) {
        const QString message(tr("Connecting to the given sever failed. See the log for details."));
        qDebug()<<"Failed to connect to server";
        return;
    }
    connect(mOpcUaClient, &QOpcUaClient::endpointsRequestFinished, this, &ConnectServer::getEndpointsComplete);
    connect(mOpcUaClient, &QOpcUaClient::connected, this, &ConnectServer::clientConnected);

    mOpcUaClient->requestEndpoints(serverUrl);
}

void ConnectServer::findServers()
{
    QStringList localeIds;
    QStringList serverUris;
    QUrl url("opc.tcp://localhost:43344");


    mOpcUaClient = mOpcUaProvider->createClient("open62541");
    if (!mOpcUaClient) {
        const QString message(tr("Connecting to the given sever failed. See the log for details."));
        qDebug()<<"Failed to connect to server";
        return;
    }
    connect(mOpcUaClient, &QOpcUaClient::endpointsRequestFinished, this, &ConnectServer::getEndpointsComplete);
    connect(mOpcUaClient, &QOpcUaClient::connected, this, &ConnectServer::clientConnected);

    // set default port if missing
    if (url.port() == -1) url.setPort(4840);

    if (mOpcUaClient) {
        mOpcUaClient->findServers(url, localeIds, serverUris);
        qDebug() << "Discovering servers on " << url.toString();
    }
}

OpcUaModel *ConnectServer::returnModel()
{
    qDebug()<<"return model";
    return mOpcUaModel;
}

bool ConnectServer::sucs() const
{
    return m_sucs;
}

void ConnectServer::getEndpointsComplete(const QVector<QOpcUaEndpointDescription> &endpoints, QOpcUa::UaStatusCode statusCode)
{
    const char *modes[] = {
        "Invalid",
        "None",
        "Sign",
        "SignAndEncrypt"
    };

    if (isSuccessStatus(statusCode)) {
        mEndpointList = endpoints;
        qDebug()<<"size of endpointis :"<<mEndpointList.size();
        for (const auto &endpoint : endpoints) {
            if (endpoint.securityMode() > sizeof(modes)) {
                qWarning() << "Invalid security mode";
                continue;
            }
        }
    }

}

void ConnectServer::clientConnected()
{
    mClientConnected = true;

    connect(mOpcUaClient, &QOpcUaClient::namespaceArrayUpdated, this, &ConnectServer::namespacesArrayUpdated);
    mOpcUaClient->updateNamespaceArray();
}

void ConnectServer::namespacesArrayUpdated(const QStringList &namespaceArray)
{
    if (namespaceArray.isEmpty()) {
        qWarning() << "Failed to retrieve the namespaces array";
        return;
    }

    disconnect(mOpcUaClient, &QOpcUaClient::namespaceArrayUpdated, this, &ConnectServer::namespacesArrayUpdated);
    mOpcUaModel->setOpcUaClient(mOpcUaClient);
    qDebug()<<"row:"<<mOpcUaModel->columnCount();
    m_sucs =true;
    emit sucsChanged(m_sucs);
    //ui->treeView->header()->setSectionResizeMode(1 /* Value column*/, &QConnectServer::Interactive);
}
