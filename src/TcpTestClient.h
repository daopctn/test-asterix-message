#ifndef TCP_TEST_CLIENT_H
#define TCP_TEST_CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QElapsedTimer>
#include "AsterixMessage.h"
#include "TimingStats.h"

class TcpTestClient : public QObject
{
    Q_OBJECT

public:
    static constexpr const char* DEFAULT_HOST = "127.0.0.1";
    static constexpr quint16 DEFAULT_PORT = 3000;
    static constexpr int CONNECTION_TIMEOUT_MS = 5000;
    static constexpr int WRITE_TIMEOUT_MS = 3000;

    explicit TcpTestClient(QObject *parent = nullptr);
    ~TcpTestClient();

    bool connectToServer();
    void disconnectFromServer();
    bool isConnected() const;

    bool sendMessage(const AsterixMessage &message);
    bool sendRawData(const QByteArray &data);

    TimingStats lastTimingStats() const;
    int totalMessagesSent() const;
    qint64 totalBytesSent() const;

    void setHost(const QString &host);
    void setPort(quint16 port);
    QString host() const;
    quint16 port() const;

signals:
    void connected();
    void disconnected();
    void messageSent(const QByteArray &data, qint64 elapsedMs);
    void errorOccurred(const QString &errorString);

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);
    void onBytesWritten(qint64 bytes);

private:
    QTcpSocket *m_socket;
    QString m_host;
    quint16 m_port;

    int m_messageCount;
    qint64 m_totalBytes;
    TimingStats m_lastStats;
    QElapsedTimer m_connectionTimer;
    QElapsedTimer m_sendTimer;

    bool m_waitingForWrite;
    qint64 m_pendingBytes;
};

#endif // TCP_TEST_CLIENT_H
