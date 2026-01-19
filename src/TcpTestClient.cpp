#include "TcpTestClient.h"
#include <QEventLoop>
#include <QTimer>

TcpTestClient::TcpTestClient(QObject *parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_host(DEFAULT_HOST)
    , m_port(DEFAULT_PORT)
    , m_messageCount(0)
    , m_totalBytes(0)
    , m_waitingForWrite(false)
    , m_pendingBytes(0)
{
    connect(m_socket, &QTcpSocket::connected,
            this, &TcpTestClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected,
            this, &TcpTestClient::onDisconnected);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &TcpTestClient::onError);
    connect(m_socket, &QTcpSocket::bytesWritten,
            this, &TcpTestClient::onBytesWritten);
}

TcpTestClient::~TcpTestClient()
{
    if (m_socket && m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
        if (m_socket->state() != QAbstractSocket::UnconnectedState) {
            m_socket->waitForDisconnected(1000);
        }
    }
}

bool TcpTestClient::connectToServer()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        return true;
    }

    m_connectionTimer.start();
    m_socket->connectToHost(m_host, m_port);

    if (!m_socket->waitForConnected(CONNECTION_TIMEOUT_MS)) {
        emit errorOccurred(QString("Connection timeout: %1").arg(m_socket->errorString()));
        return false;
    }

    m_lastStats.connectionTimeMs = m_connectionTimer.elapsed();
    return true;
}

void TcpTestClient::disconnectFromServer()
{
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
        if (m_socket->state() != QAbstractSocket::UnconnectedState) {
            m_socket->waitForDisconnected(1000);
        }
    }
}

bool TcpTestClient::isConnected() const
{
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

bool TcpTestClient::sendMessage(const AsterixMessage &message)
{
    if (!message.isValid()) {
        emit errorOccurred("Invalid ASTERIX message");
        return false;
    }
    return sendRawData(message.toByteArray());
}

bool TcpTestClient::sendRawData(const QByteArray &data)
{
    if (!isConnected()) {
        emit errorOccurred("Not connected to server");
        return false;
    }

    m_sendTimer.start();
    qint64 bytesWritten = m_socket->write(data);

    if (bytesWritten == -1) {
        emit errorOccurred(QString("Write failed: %1").arg(m_socket->errorString()));
        return false;
    }

    if (!m_socket->waitForBytesWritten(WRITE_TIMEOUT_MS)) {
        emit errorOccurred("Write timeout");
        return false;
    }

    m_lastStats.sendTimeMs = m_sendTimer.elapsed();
    m_lastStats.bytesSent = bytesWritten;
    m_messageCount++;
    m_totalBytes += bytesWritten;

    emit messageSent(data, m_lastStats.sendTimeMs);
    return true;
}

TimingStats TcpTestClient::lastTimingStats() const
{
    return m_lastStats;
}

int TcpTestClient::totalMessagesSent() const
{
    return m_messageCount;
}

qint64 TcpTestClient::totalBytesSent() const
{
    return m_totalBytes;
}

void TcpTestClient::setHost(const QString &host)
{
    m_host = host;
}

void TcpTestClient::setPort(quint16 port)
{
    m_port = port;
}

QString TcpTestClient::host() const
{
    return m_host;
}

quint16 TcpTestClient::port() const
{
    return m_port;
}

void TcpTestClient::onConnected()
{
    emit connected();
}

void TcpTestClient::onDisconnected()
{
    emit disconnected();
}

void TcpTestClient::onError(QAbstractSocket::SocketError socketError)
{
    QString errorMsg;
    switch (socketError) {
        case QAbstractSocket::ConnectionRefusedError:
            errorMsg = "Connection refused - is the server running?";
            break;
        case QAbstractSocket::HostNotFoundError:
            errorMsg = "Host not found";
            break;
        case QAbstractSocket::SocketTimeoutError:
            errorMsg = "Connection timed out";
            break;
        case QAbstractSocket::NetworkError:
            errorMsg = "Network error";
            break;
        default:
            errorMsg = m_socket->errorString();
    }
    emit errorOccurred(errorMsg);
}

void TcpTestClient::onBytesWritten(qint64 bytes)
{
    m_pendingBytes -= bytes;
    if (m_pendingBytes <= 0) {
        m_waitingForWrite = false;
    }
}
