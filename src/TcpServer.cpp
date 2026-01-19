#include "TcpServer.h"
#include <QHostAddress>
#include <QTextStream>

TcpServer::TcpServer(QObject *parent)
    : QObject(parent)
    , m_server(new QTcpServer(this))
    , m_host(DEFAULT_HOST)
    , m_port(DEFAULT_PORT)
    , m_messageCount(0)
    , m_totalBytes(0)
{
    connect(m_server, &QTcpServer::newConnection,
            this, &TcpServer::onNewConnection);
}

TcpServer::~TcpServer()
{
    stop();
}

bool TcpServer::start(const QString &host, quint16 port)
{
    m_host = host;
    m_port = port;

    QHostAddress address(host);

    if (!m_server->listen(address, port)) {
        emit errorOccurred(QString("Failed to start server: %1").arg(m_server->errorString()));
        return false;
    }

    return true;
}

void TcpServer::stop()
{
    if (m_server->isListening()) {
        // Disconnect all clients
        for (QTcpSocket *socket : m_clients.keys()) {
            socket->disconnectFromHost();
            socket->deleteLater();
        }
        m_clients.clear();

        m_server->close();
    }
}

bool TcpServer::isListening() const
{
    return m_server->isListening();
}

QString TcpServer::serverAddress() const
{
    if (m_server->isListening()) {
        return m_server->serverAddress().toString();
    }
    return QString();
}

quint16 TcpServer::serverPort() const
{
    return m_server->serverPort();
}

int TcpServer::totalMessagesReceived() const
{
    return m_messageCount;
}

qint64 TcpServer::totalBytesReceived() const
{
    return m_totalBytes;
}

void TcpServer::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket *socket = m_server->nextPendingConnection();

        QString clientAddress = QString("%1:%2")
            .arg(socket->peerAddress().toString())
            .arg(socket->peerPort());

        m_clients[socket] = clientAddress;

        connect(socket, &QTcpSocket::disconnected,
                this, &TcpServer::onClientDisconnected);
        connect(socket, &QTcpSocket::readyRead,
                this, &TcpServer::onReadyRead);
        connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
                this, &TcpServer::onSocketError);

        emit clientConnected(clientAddress);
    }
}

void TcpServer::onClientDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QString clientAddress = m_clients.value(socket, "Unknown");
    m_clients.remove(socket);

    emit clientDisconnected(clientAddress);

    socket->deleteLater();
}

void TcpServer::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    m_totalBytes += data.size();

    // Decode the ASTERIX message
    DecodedAsterixMessage decoded = AsterixDecoder::decode(data);

    if (decoded.isValid) {
        m_messageCount++;
    }

    emit messageReceived(decoded);
}

void TcpServer::onSocketError(QAbstractSocket::SocketError socketError)
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QString errorMsg;
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
            // Normal disconnection, don't emit error
            return;
        case QAbstractSocket::NetworkError:
            errorMsg = "Network error";
            break;
        default:
            errorMsg = socket->errorString();
    }

    emit errorOccurred(QString("Socket error: %1").arg(errorMsg));
}
