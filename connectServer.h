#ifndef CONNECTSERVER_H
#define CONNECTSERVER_H

#include <QObject>
#include "opcuamodel.h"
#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QTextCharFormat>
#include <QTextBlock>
#include <QOpcUaProvider>
#include <QOpcUaAuthenticationInformation>
#include <QOpcUaErrorState>

#include <QOpcUaClient>


class QOpcUaProvider;
class OpcUaModel;

class ConnectServer : public QObject
{
    Q_OBJECT
public:
    ConnectServer(QObject *parent = nullptr);
    ~ConnectServer();

    Q_INVOKABLE void connectToServer();

    Q_INVOKABLE void getEndpoint();
    Q_INVOKABLE void findServers();
    Q_INVOKABLE OpcUaModel* returnModel();

    Q_PROPERTY(bool sucs READ sucs NOTIFY sucsChanged)

    bool sucs() const;

signals:
    void sucsChanged(bool state);

private slots:
    void getEndpointsComplete(const QVector<QOpcUaEndpointDescription> &endpoints, QOpcUa::UaStatusCode statusCode);
    void clientConnected();
    void namespacesArrayUpdated(const QStringList &namespaceArray);


private:
    OpcUaModel *mOpcUaModel;
    QOpcUaProvider *mOpcUaProvider;
    QOpcUaClient *mOpcUaClient = nullptr;
    QVector<QOpcUaEndpointDescription> mEndpointList;
    bool mClientConnected = false;
    QOpcUaApplicationIdentity m_identity;
    QOpcUaPkiConfiguration m_pkiConfig;
    QOpcUaEndpointDescription m_endpoint;
    bool m_sucs;
};

#endif // CONNECTSERVER_H
