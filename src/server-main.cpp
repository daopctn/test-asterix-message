#include <QCoreApplication>
#include <QTextStream>
#include <csignal>
#include "TcpServer.h"
#include "HexDumper.h"

static TcpServer *g_server = nullptr;

void signalHandler(int signal)
{
    QTextStream cout(stdout);
    cout << "\n\nReceived signal " << signal << ", shutting down server..." << Qt::endl;

    if (g_server) {
        g_server->stop();
    }

    QCoreApplication::quit();
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("asterix-tcp-server");
    QCoreApplication::setApplicationVersion("1.0.0");

    // Install signal handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    QTextStream cout(stdout);
    cout << "╔════════════════════════════════════════════╗" << Qt::endl;
    cout << "║     ASTERIX TCP Server v1.0                ║" << Qt::endl;
    cout << "║     Listening for ASTERIX messages         ║" << Qt::endl;
    cout << "╚════════════════════════════════════════════╝" << Qt::endl;
    cout << "Qt Version: " << QT_VERSION_STR << Qt::endl;
    cout << Qt::endl;

    TcpServer server;
    g_server = &server;

    // Connect signals to display messages
    QObject::connect(&server, &TcpServer::clientConnected, [&cout](const QString &address) {
        cout << "\n[CLIENT CONNECTED] " << address << Qt::endl;
        cout << "────────────────────────────────────────────" << Qt::endl;
    });

    QObject::connect(&server, &TcpServer::clientDisconnected, [&cout](const QString &address) {
        cout << "\n[CLIENT DISCONNECTED] " << address << Qt::endl;
        cout << "────────────────────────────────────────────" << Qt::endl;
    });

    QObject::connect(&server, &TcpServer::messageReceived, [&cout, &server](const DecodedAsterixMessage &message) {
        static int messageNumber = 0;
        messageNumber++;

        cout << "\n╔════════════════════════════════════════════╗" << Qt::endl;
        cout << "║  MESSAGE #" << QString("%1").arg(messageNumber, -33) << "║" << Qt::endl;
        cout << "╚════════════════════════════════════════════╝" << Qt::endl;

        if (!message.isValid) {
            cout << "\n[ERROR] Invalid message received!" << Qt::endl;
            cout << "Error: " << message.errorMessage << Qt::endl;
        } else {
            cout << message.toDetailedString() << Qt::endl;

            // Show full hex dump of the complete message
            QByteArray fullMessage;
            fullMessage.append(static_cast<char>(message.category));
            fullMessage.append(static_cast<char>((message.length >> 8) & 0xFF));
            fullMessage.append(static_cast<char>(message.length & 0xFF));
            fullMessage.append(message.fspec);
            fullMessage.append(message.dataItems);

            cout << "\nComplete message hex dump:" << Qt::endl;
            cout << HexDumper::dump(fullMessage) << Qt::endl;
        }

        cout << "\n[STATISTICS]" << Qt::endl;
        cout << "  Total messages received: " << server.totalMessagesReceived() << Qt::endl;
        cout << "  Total bytes received:    " << server.totalBytesReceived() << Qt::endl;
        cout << "\n════════════════════════════════════════════" << Qt::endl;
        cout.flush();
    });

    QObject::connect(&server, &TcpServer::errorOccurred, [&cout](const QString &error) {
        cout << "\n[ERROR] " << error << Qt::endl;
    });

    // Start the server
    if (!server.start(TcpServer::DEFAULT_HOST, TcpServer::DEFAULT_PORT)) {
        cout << "[FATAL] Failed to start server!" << Qt::endl;
        return 1;
    }

    cout << "[SERVER STARTED]" << Qt::endl;
    cout << "  Address: " << server.serverAddress() << Qt::endl;
    cout << "  Port:    " << server.serverPort() << Qt::endl;
    cout << Qt::endl;
    cout << "Waiting for ASTERIX messages..." << Qt::endl;
    cout << "(Press Ctrl+C to stop)" << Qt::endl;
    cout << "════════════════════════════════════════════" << Qt::endl;
    cout.flush();

    // Run the event loop
    int result = app.exec();

    cout << "\nServer stopped." << Qt::endl;
    return result;
}
