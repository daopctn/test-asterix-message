#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include "AsterixDecoder.h"

class TcpServer : public QObject
{
    Q_OBJECT

public:
    static constexpr const char* DEFAULT_HOST = "127.0.0.1";
    static constexpr quint16 DEFAULT_PORT = 3000;

    explicit TcpServer(QObject *parent = nullptr);
    ~TcpServer();

    bool start(const QString &host = DEFAULT_HOST, quint16 port = DEFAULT_PORT);
    void stop();

    bool isListening() const;
    QString serverAddress() const;
    quint16 serverPort() const;

    int totalMessagesReceived() const;
    qint64 totalBytesReceived() const;

signals:
    void messageReceived(const DecodedAsterixMessage &message);
    void clientConnected(const QString &address);
    void clientDisconnected(const QString &address);
    void errorOccurred(const QString &errorString);

private slots:
    void onNewConnection();
    void onClientDisconnected();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    QTcpServer *m_server;
    QString m_host;
    quint16 m_port;

    QMap<QTcpSocket*, QString> m_clients;
    int m_messageCount;
    qint64 m_totalBytes;
};

#endif // TCP_SERVER_H
